/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_sigevent.h"
#include "socket_util.h"

int recv_send_udp_local_ip(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_sockaddr_in dest_addr = {0};

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	dest_addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) == -1) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

static void notify_udp_ipv4(union nfp_sigval *sv);
int socket_sigevent_udp4(int fd)
{
	struct nfp_sigevent ev = {0};
	struct nfp_sockaddr_in dest_addr = {0};
	const char *buf = "sigevent_test";

	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify_udp_ipv4;
	ev.sigev_value.sival_ptr = NULL;
	nfp_socket_sigevent(fd, &ev);

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	dest_addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) == -1) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}
	sleep(2);
	return 0;
}

static void notify_udp_ipv4(union nfp_sigval *sv)
{
	struct nfp_sockaddr_in addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);
	int data_len = 0;
	uint8_t *data = NULL;
	struct nfp_sock_sigval *ss;
	int i;

	ss = (struct nfp_sock_sigval *)sv;

	data = nfp_udp_packet_parse(ss->pkt, &data_len,
		(struct nfp_sockaddr *)&addr,
		&addr_len);

	NFP_INFO("UDP data received: size %d, data: ", data_len);

	for (i = 0; i < data_len; i++)
		NFP_LOG_NO_CTX(NFP_LOG_INFO, "%c", data[i]);

	NFP_LOG_NO_CTX(NFP_LOG_INFO, "\n");

	if (addr_len != sizeof(addr)) {
		NFP_ERR("Failed to rcv source address: %d (errno = %d)\n",
			addr_len, nfp_errno);
		return;
	}

	NFP_INFO("Data was received from address 0x%x, port = %d.\n",
		odp_be_to_cpu_32(addr.sin_addr.s_addr),
		odp_be_to_cpu_16(addr.sin_port));
	/*
	 * Mark ss->pkt invalid to indicate it was released or reused by us.
	 */
	odp_packet_free(ss->pkt);
	ss->pkt = ODP_PACKET_INVALID;
	NFP_INFO("SUCCESS.\n");
}

#ifdef INET6
int recv_send_udp6_local_ip(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_sockaddr_in6 dest_addr = {0};

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, dest_addr.sin6_addr.nfp_s6_addr);

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) == -1) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

static void notify_udp_ipv6(union nfp_sigval *sv);
int socket_sigevent_udp6(int fd)
{
	struct nfp_sigevent ev;
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "sigevent_test";

	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify_udp_ipv6;
	ev.sigev_value.sival_ptr = NULL;
	nfp_socket_sigevent(fd, &ev);

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, dest_addr.sin6_addr.nfp_s6_addr);

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) == -1) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	sleep(2);
	return 0;
}

static void notify_udp_ipv6(union nfp_sigval *sv)
{
	struct nfp_sockaddr_in6 addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);
	int data_len = 0;
	uint8_t *data = NULL;
	struct nfp_sock_sigval *ss;
	int i;

	ss = (struct nfp_sock_sigval *)sv;

	data = nfp_udp_packet_parse(ss->pkt, &data_len,
		(struct nfp_sockaddr *)&addr,
		&addr_len);

	NFP_INFO("UDP data received: size %d, data: ", data_len);

	for (i = 0; i < data_len; i++)
		NFP_LOG_NO_CTX(NFP_LOG_INFO, "%c", data[i]);

	NFP_LOG_NO_CTX(NFP_LOG_INFO, "\n");

	if (addr_len != sizeof(addr)) {
		NFP_ERR("Failed to rcv source address: %d (errno = %d)\n",
			addr_len, nfp_errno);
		return;
	}

	NFP_INFO("Address: %x:%x:%x:%x, port: %d.\n",
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[0]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[1]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[2]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[3]),
		odp_be_to_cpu_16(addr.sin6_port));
	/*
	 * Mark ss->pkt invalid to indicate it was released or reused by us.
	 */
	ss->pkt = ODP_PACKET_INVALID;
	NFP_INFO("SUCCESS.\n");
}
#endif /* INET6 */

int connect_recv_send_tcp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	len = nfp_send(fd, buf, (nfp_size_t)len, 0);
	if (len == -1) {
		NFP_ERR("Failed to send data. (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int connect_recv_send_tcp6_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	len = nfp_send(fd, buf, (nfp_size_t)len, 0);
	if (len == -1) {
		NFP_ERR("Failed to send data. (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /*INET6*/

static void notify_tcp_rcv(union nfp_sigval *sv);
int socket_sigevent_tcp_rcv(int fd)
{
	struct nfp_sigevent ev = {0};
	const char *buf = "socket_test";
	int len = 0;
	int fd_accept = -1;

	fd_accept = nfp_accept(fd, NULL, NULL);
	if (fd_accept == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify_tcp_rcv;
	ev.sigev_value.sival_ptr = NULL;
	if (nfp_socket_sigevent(fd_accept, &ev) == -1) {
		NFP_ERR("Failed to set sigevent(errno = %d)\n", nfp_errno);
		return -1;
	}

	len = nfp_send(fd_accept, buf, strlen(buf) + 1, 0);
	if (len == -1) {
		NFP_ERR("Failed to send data. (errno = %d)\n", nfp_errno);
		return -1;
	}
	sleep(3);
	nfp_close(fd_accept);
	NFP_INFO("Socket sigevent set.\n");
	return 0;
}

static void notify_tcp_rcv(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss;
	uint8_t *data = NULL;
	int data_len = 0;
	int i;

	ss = (struct nfp_sock_sigval *)sv;

	if (ss->event != NFP_EVENT_RECV)
		return;

	data = odp_packet_data(ss->pkt);
	data_len = odp_packet_len(ss->pkt);

	NFP_INFO("TCP data received: size %d, data: ", data_len);

	for (i = 0; i < data_len; i++)
		NFP_LOG_NO_CTX(NFP_LOG_INFO, "%c", data[i]);

	NFP_LOG_NO_CTX(NFP_LOG_INFO, "\n");

	NFP_INFO("SUCCESS.\n");
}

int connect_tcp_delayed_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int connect_tcp6_delayed_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

static void notify_tcp_accept(union nfp_sigval *sv);
int socket_sigevent_tcp_accept(int fd)
{
	struct nfp_sigevent ev;
	int fd_accept = -1;

	ev.sigev_notify = NFP_SIGEV_HOOK;
	ev.sigev_notify_func = notify_tcp_accept;
	ev.sigev_value.sival_ptr = NULL;
	if (nfp_socket_sigevent(fd, &ev) == -1) {
		NFP_ERR("Failed to set sigevent(errno = %d)\n", nfp_errno);
		return -1;
	}
	NFP_INFO("Socket sigevent set.\n");

	fd_accept = nfp_accept(fd, NULL, NULL);
	if (fd_accept == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}
	if (nfp_close(fd_accept) == -1) {
		NFP_ERR("Failed to close connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

static void notify_tcp_accept(union nfp_sigval *sv)
{
	struct nfp_sock_sigval *ss;

	ss = (struct nfp_sock_sigval *)sv;

	if (ss->event != NFP_EVENT_ACCEPT)
		return;

	NFP_INFO("TCP Connection received on socket %d: %d created.\n",
		ss->sockfd,
		ss->sockfd2);

	NFP_INFO("SUCCESS.\n");
}

