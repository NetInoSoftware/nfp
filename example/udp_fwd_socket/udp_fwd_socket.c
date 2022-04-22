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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nfp.h"
#include "udp_fwd_socket.h"

struct nfp_sockaddr_in *raddr = NULL;
int *sock_array;
int sock_array_size;

static void notify(union nfp_sigval *sv);

static int create_local_sock(int lport, char *laddr_txt)
{
	int sd;
	struct nfp_sockaddr_in laddr = {0};
	struct nfp_sigevent ev;

	/* Create socket*/
	if ((sd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP))
		< 0) {
		NFP_ERR("Error: Failed to create socket: errno = %s!\n",
			 nfp_strerror(nfp_errno));
		return -1;
	}

	memset(&laddr, 0, sizeof(laddr));
	laddr.sin_family = NFP_AF_INET;
	laddr.sin_port = odp_cpu_to_be_16(lport);
	laddr.sin_addr.s_addr = inet_addr(laddr_txt);
	laddr.sin_len = sizeof(laddr);

	/* Bind to local address*/
	if (nfp_bind(sd, (struct nfp_sockaddr *)&laddr,
		       sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("Error: Failed to bind: addr=%s, port=%d: errno=%s\n",
			laddr_txt, TEST_LPORT, nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	/* Register callback on socket*/
	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify;
	ev.sigev_value.sival_ptr = NULL;

	if (nfp_socket_sigevent(sd, &ev) == -1) {
		NFP_ERR("Error: Failed configure socket callback: errno = %s\n",
			nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	return sd;
}

int udp_fwd_cfg(int sock_count, char *laddr_txt, char *raddr_txt)
{
	int port_idx;
	int sd = -1;

	sock_array_size = 0;
	sock_array = (int *)malloc(sock_count * sizeof(int));
	if (sock_array == NULL) {
		NFP_ERR("Error: Failed allocate memory\n");
		return -1;
	}

	for (port_idx = 0; port_idx < sock_count; port_idx++) {
		sd = create_local_sock(TEST_LPORT + port_idx, laddr_txt);
		if (sd == -1)
			return -1;
		sock_array[sock_array_size] = sd;
		sock_array_size++;
	}

	/* Allocate remote address - will be used in notification function*/
	raddr = malloc(sizeof(struct nfp_sockaddr_in));
	if (raddr == NULL) {
		NFP_ERR("Error: Failed allocate memory\n");
		return -1;
	}
	memset(raddr, 0, sizeof(*raddr));
	raddr->sin_family = NFP_AF_INET;
	raddr->sin_port = odp_cpu_to_be_16(TEST_RPORT);
	raddr->sin_addr.s_addr = inet_addr(raddr_txt);
	raddr->sin_len = sizeof(*raddr);

	return 0;
}



static void notify(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss = (struct nfp_sock_sigval *)sv;
	int s = ss->sockfd;
	if (ss->event != NFP_EVENT_RECV)
		return;

	nfp_udp_packet_parse(ss->pkt, NULL, NULL, NULL);

	nfp_udp_packet_sendto(s, ss->pkt,
			      (struct nfp_sockaddr *)raddr, sizeof(*raddr));

	/* mark packet as consumed*/
	ss->pkt = ODP_PACKET_INVALID;
}

int udp_fwd_cleanup(void)
{
	int i;

	if (raddr) {
		free(raddr);
		raddr = NULL;
	}

	if (sock_array_size) {
		for (i = 0; i < sock_array_size; i++)
			nfp_close(sock_array[i]);
	}

	if (sock_array) {
		free(sock_array);
		sock_array = NULL;
	}

	return 0;
}

enum nfp_return_code packet_hook_ipv4(odp_packet_t pkt, void *arg)
{
	struct nfp_ip *ip = (struct nfp_ip *)odp_packet_l3_ptr(pkt, NULL);
	struct nfp_udphdr *udp;
	uint16_t lport;

	(void)arg;

	if (ip->ip_p != NFP_IPPROTO_UDP)
		return NFP_PKT_CONTINUE;

	udp = (struct nfp_udphdr *)((uint8_t *)ip + (ip->ip_hl << 2));
	lport = odp_be_to_cpu_16(udp->uh_dport);

	if (lport < TEST_LPORT || lport > TEST_LPORT + sock_array_size - 1)
		return NFP_PKT_CONTINUE;

	ip->ip_dst.s_addr = raddr->sin_addr.s_addr;
	ip->ip_sum = 0;
	udp->uh_dport = raddr->sin_port;
	udp->uh_sum = 0;

	return nfp_ip_send(pkt, NULL);
}

enum nfp_return_code packet_hook_udpv4(odp_packet_t pkt, void *arg)
{
	struct nfp_ip *ip = (struct nfp_ip *)odp_packet_l3_ptr(pkt, NULL);
	struct nfp_udphdr *udp;
	uint16_t lport;

	(void)arg;

	udp = (struct nfp_udphdr *)((uint8_t *)ip + (ip->ip_hl << 2));
	lport = odp_be_to_cpu_16(udp->uh_dport);

	if (lport < TEST_LPORT || lport > TEST_LPORT + sock_array_size - 1)
		return NFP_PKT_CONTINUE;

	ip->ip_dst.s_addr = raddr->sin_addr.s_addr;
	ip->ip_sum = 0;
	udp->uh_dport = raddr->sin_port;
	udp->uh_sum = 0;

	return nfp_ip_send(pkt, NULL);
}

