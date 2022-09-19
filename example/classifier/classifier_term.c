/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */
#include <unistd.h>
#include "classifier_term.h"

extern uint64_t received_total;
extern uint64_t received_pkt_total;

int init_posix(uint16_t port)
{
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	struct nfp_sockaddr_in addr = {0};
	uint32_t ip_addr = 0;
	int fd_rcv = -1;
	int res = -1;
	int retry = 10;

	/* Bind it to the address from first interface */
	ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
	if (ifnet == NFP_IFNET_INVALID) {
		NFP_ERR("Interface not found.");
		return -1;
	}

	fd_rcv = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (fd_rcv == -1) {
		NFP_ERR("Failed to create RCV socket (errno = %d)", nfp_errno);
		return -1;
	}

	if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
				    &ip_addr)) {
		NFP_ERR("Failed to get IP address.");
		nfp_close(fd_rcv);
		return -1;
	}

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(port);
	addr.sin_addr.s_addr = ip_addr;

	do {
		res = nfp_bind(fd_rcv, (const struct nfp_sockaddr *)&addr,
			       sizeof(struct nfp_sockaddr_in));
		retry--;
		if (res == -1)
			sleep(1);
	} while (res == -1 && retry > 0);

	if (res == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n", nfp_errno);
		nfp_close(fd_rcv);
		return -1;
	}

	return fd_rcv;
}

int cleanup_posix(int sd)
{
	if (sd == -1)
		return 0;

	return nfp_close(sd);
}

int termination_posix(void *arg)
{
	odp_bool_t *is_running = NULL;
	nfp_ssize_t len = 0;
	char buf[1500];
	nfp_size_t buf_len = sizeof(buf);
	int fd_rcv = *(int *)arg;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		len = nfp_recv(fd_rcv, buf, buf_len, NFP_MSG_DONTWAIT);
		if (len == -1) {
			if (nfp_errno == NFP_EWOULDBLOCK)
				continue;

			NFP_ERR("Failed to receive data (errno = %d)",
				nfp_errno);
			break;
		}
		received_total += len;
		received_pkt_total++;
		NFP_DBG("Data received: length = %d.", len);
	}

	return 0;
}

nfp_channel_t init_channel(void)
{
	nfp_channel_param_t param;

	nfp_channel_param_init(&param);

	param.method = NFP_CHANNEL_METHOD_TAKE;
	param.ua_location = NFP_CHANNEL_UA_LOCATION_PKT_UA;
	param.queue_param.deq_mode = ODP_QUEUE_OP_MT_UNSAFE;
	param.queue_param.enq_mode = ODP_QUEUE_OP_MT_UNSAFE;

	return nfp_channel_create("udp_channel", &param);
}

int cleanup_channel(nfp_channel_t chn)
{
	if (chn == NFP_CHANNEL_INVALID)
		return 0;

	return nfp_channel_destroy(chn);
}

void channel_packet_input(nfp_pkt_vector_t *vec, nfp_channel_t chn)
{
	struct nfp_udphdr *uh;
	uint8_t *udp_data;
	uint16_t udp_size, i, vec_num;
	odp_packet_t pkt;
	int pkt_sent = 0;
	uint32_t off;

	if (chn == NFP_CHANNEL_INVALID || !vec)
		return;

	vec_num = vec->num;

	if (!vec_num)
		return;

	for (i = 0; i < vec_num; i++) {
		pkt = vec->pkt[i];

		uh = (struct nfp_udphdr *)odp_packet_l4_ptr(pkt, NULL);
		udp_size = odp_be_to_cpu_16(uh->uh_ulen) -
					    sizeof(struct nfp_udphdr);

		udp_data = (uint8_t *)(uh + 1);

		off = (uint32_t)(udp_data - (uint8_t *)odp_packet_data(pkt));
		/* trim the packet to the length of udp data */
		odp_packet_pull_head(pkt, off);
		odp_packet_pull_tail(pkt, odp_packet_len(pkt) - udp_size);
	}

	pkt_sent = nfp_channel_send(chn, vec->pkt, NULL, 0, vec_num);

	if (pkt_sent != vec_num) {
		uint16_t start_idx;

		if (pkt_sent == -1)
			start_idx = 0;
		else
			start_idx = pkt_sent;
		for (i = start_idx; i < vec_num; i++)
			odp_packet_free(vec->pkt[i]);

		NFP_DBG("Error: Failed to send packet on the channel");
	}
}

#define CHANNEL_RECV_MAX 32
int termination_channel(void *arg)
{
	nfp_channel_t chn;
	odp_bool_t *is_running = NULL;
	int recv, i;
	odp_packet_t pkt[CHANNEL_RECV_MAX], p;

	if (!arg) {
		NFP_ERR("Error: Invalid argument");
		return -1;
	}

	chn = *(nfp_channel_t *)arg;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		recv = nfp_channel_recv(chn, pkt, NULL, 0, CHANNEL_RECV_MAX);
		if (recv > 0) {
			for (i = 0; i < recv; i++) {
				p = pkt[i];

				received_total += odp_packet_len(p);
				received_pkt_total++;

				NFP_DBG("Data received(chn): length = %d.",
					odp_packet_len(p));
				odp_packet_free(p);
			}
		}
	}

	return 0;
}

