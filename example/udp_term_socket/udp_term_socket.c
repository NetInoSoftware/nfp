/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "unistd.h"
#include "nfp.h"
#include "udp_term_socket.h"

uint64_t received_total;
uint64_t received_pkt_total;

static void notify(union nfp_sigval *sv);

int udp_socket_cfg(char *laddr, uint16_t lport, nfp_channel_t *chn)
{
	int sd = -1;
	struct nfp_sockaddr_in addr = {0};
	struct nfp_sigevent ev = {0};

	if (!laddr || !chn) {
		NFP_ERR("Error: Invalid argument");
		return -1;
	}

	sd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (sd == -1) {
		NFP_ERR("Error: Failed to create socket: errno = %s",
			nfp_strerror(nfp_errno));
		return -1;
	}

	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(lport);
	if (nfp_parse_ip_addr(laddr, &addr.sin_addr.s_addr) != 1) {
		NFP_ERR("Error: Failed to parse the address: %s", laddr);
		return -1;
	}
	addr.sin_len = sizeof(addr);

	/* Bind to local address*/
	if (nfp_bind(sd, (struct nfp_sockaddr *)&addr,
		     sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("Error: Failed to bind: addr=%s, port=%d: errno=%s\n",
			laddr, lport, nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	if (*chn != NFP_CHANNEL_INVALID) { /* METHOD_CHANNEL */
		/* Register callback on socket*/
		ev.sigev_notify = NFP_SIGEV_HOOK;
		ev.sigev_notify_func = notify;
		ev.sigev_value.sival_ptr = chn;

		if (nfp_socket_sigevent(sd, &ev) == -1) {
			NFP_ERR("Error: Failed configure socket callback: "
				"errno = %s", nfp_strerror(nfp_errno));
			nfp_close(sd);
			return -1;
		}
	}

	return sd;
}

static void notify(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss = (struct nfp_sock_sigval *)sv;
	nfp_channel_t chn = *(nfp_channel_t *)ss->sigev_value.sival_ptr;
	odp_packet_t pkt;
	uint8_t *udp_data;
	uint16_t udp_size;
	struct nfp_udphdr *uh;

	if (ss->event != NFP_EVENT_RECV)
		return;

	pkt = ss->pkt;
	/* mark packet as consumed*/
	ss->pkt = ODP_PACKET_INVALID;

	uh = (struct nfp_udphdr *)odp_packet_l4_ptr(pkt, NULL);
	udp_size = odp_be_to_cpu_16(uh->uh_ulen) - sizeof(struct nfp_udphdr);

	udp_data = (uint8_t *)(uh + 1);

	/* trim the packet to the length of udp data */
	odp_packet_pull_head(pkt, (uint32_t)(udp_data -
					     (uint8_t *)odp_packet_data(pkt)));
	odp_packet_pull_tail(pkt, odp_packet_len(pkt) - udp_size);

	if (nfp_channel_send(chn, &pkt, NULL, 0, 1) != 1) {
		odp_packet_free(pkt);
		NFP_ERR("Error: Failed to send packet on the channel");
	}
}

int udp_socket_cleanup(int sd)
{
	if (sd == -1)
		return 0;

	return nfp_close(sd);
}

nfp_channel_t udp_channel_config(void)
{
	nfp_channel_param_t param;

	nfp_channel_param_init(&param);

	param.method = NFP_CHANNEL_METHOD_TAKE;
	param.ua_location = NFP_CHANNEL_UA_LOCATION_PKT_UA;
	param.queue_param.deq_mode = ODP_QUEUE_OP_MT_UNSAFE;
	param.queue_param.enq_mode = ODP_QUEUE_OP_MT;

	return nfp_channel_create("udp_channel", &param);
}

int udp_channel_cleanup(nfp_channel_t chn)
{
	if (chn == NFP_CHANNEL_INVALID)
		return 0;

	return nfp_channel_destroy(chn);
}

#define TERMINATION_BUFF_SIZE 1500
int termination_posix(void *arg)
{
	uint8_t buff[TERMINATION_BUFF_SIZE];
	odp_bool_t *is_running = NULL;
	int sd;
	nfp_ssize_t recv;

	if (!arg) {
		NFP_ERR("Error: Invalid argument");
		return -1;
	}

	sd = *(int *)arg;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		recv = nfp_recv(sd, buff, TERMINATION_BUFF_SIZE, 0);
		if (recv > 0) {
			received_total += recv;
			received_pkt_total++;
		}
	}

	return 0;
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

				odp_packet_free(p);
			}
		}
	}

	return 0;
}
