/*-
 * Copyright (c) 2014 ENEA Software AB
 * Copyright (c) 2014 Nokia
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "nfp.h"

#include "udp_server.h"

#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1

struct udpecho_s {
	/*socket descriptor */
	int sd;
} udp_echo_cfg;

static void notify(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss = (struct nfp_sock_sigval *)sv;
	int s = ss->sockfd;
	int event = ss->event;
	odp_packet_t pkt = ss->pkt;
	int n;
	struct nfp_sockaddr_in addr;
	nfp_socklen_t addr_len = sizeof(addr);

	/*
	 * Only receive events are accepted.
	 */
	if (event != NFP_EVENT_RECV)
		return;

	/*
	 * L2, L3, and L4 pointers are as they were when the packet was
	 * received. L2 and L3 areas may have ancillary data written
	 * over original headers. Only L4 pointer and data after that is valid.
	 * Note that short packets may have padding. Thus odp_packet_length()
	 * may give wrong results. Sender information is over L2 area.
	 * It is best to use function nfp_udp_packet_parse() to
	 * retrieve the information. It also sets the packet's data pointer
	 * to payload and removes padding from the end.
	 */
	uint8_t *p = nfp_udp_packet_parse(pkt, &n,
					    (struct nfp_sockaddr *)&addr,
					    &addr_len);
	/* Pointer and length are not used here. */
	(void)p;
	(void)n;

	/*
	 * There are two alternatives to send a respond.
	 */
#if 1
	/*
	 * Reuse received packet.
	 * Here we want to send the same payload back prepended with "ECHO:".
	 */
	odp_packet_push_head(pkt, 5);
	memcpy(odp_packet_data(pkt), "ECHO:", 5);
	nfp_udp_packet_sendto(s, pkt, (struct nfp_sockaddr *)&addr,
			      sizeof(addr));
#else
	/*
	 * Send using usual sendto(). Remember to free the packet.
	 */
	nfp_sendto(s, p, n, 0, (struct nfp_sockaddr *)&addr, sizeof(addr));
	odp_packet_free(pkt);
#endif
	/*
	 * Mark ss->pkt invalid to indicate it was released or reused by us.
	 */
	ss->pkt = ODP_PACKET_INVALID;
}

int udpecho_config(void *arg)
{
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	struct nfp_sigevent ev = {0};
	struct nfp_sockaddr_in my_addr;
	uint32_t my_ip_addr;

	(void)arg;

	odp_memset(&udp_echo_cfg, 0, sizeof(udp_echo_cfg));
	udp_echo_cfg.sd = INVALID_SOCKET;

	sleep(1);

	/* Create socket */
	udp_echo_cfg.sd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				     NFP_IPPROTO_UDP);
	if (udp_echo_cfg.sd < 0) {
		NFP_ERR("nfp_socket failed, err='%s'", nfp_strerror(nfp_errno));
		return -1;
	}

	/* Bind it to the address from first interface, port 2048 */
	ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
	if (ifnet == NFP_IFNET_INVALID) {
		NFP_ERR("Interface not found.");
		nfp_close(udp_echo_cfg.sd);
		return -1;
	}

	if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
				    &my_ip_addr)) {
		NFP_ERR("Faile to get IP address.");
		nfp_close(udp_echo_cfg.sd);
		return -1;
	}

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = NFP_AF_INET;
	my_addr.sin_port = odp_cpu_to_be_16(UDP_LPORT);
	my_addr.sin_addr.s_addr = my_ip_addr;
	my_addr.sin_len = sizeof(my_addr);

	if (nfp_bind(udp_echo_cfg.sd, (struct nfp_sockaddr *)&my_addr,
		     sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("nfp_bind failed, err='%s'",
			 nfp_strerror(nfp_errno));
		nfp_close(udp_echo_cfg.sd);
		return -1;
	}

	/* configure sigevent */
	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify;
	ev.sigev_value.sival_ptr = NULL;

	if (nfp_socket_sigevent(udp_echo_cfg.sd, &ev)) {
		NFP_ERR("nfp_socket_sigevent failed, err='%s'",
			nfp_strerror(nfp_errno));
		nfp_close(udp_echo_cfg.sd);
		return -1;
	}

	return 0;
}

int udpecho_cleanup(void)
{
	if (udp_echo_cfg.sd != INVALID_SOCKET) {
		nfp_close(udp_echo_cfg.sd);
		udp_echo_cfg.sd = INVALID_SOCKET;
	}
	return 0;
}

