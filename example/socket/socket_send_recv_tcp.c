/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_send_recv_tcp.h"
#include "socket_util.h"

#define TCP_CYCLES 20
static const char *tcp_buf = "1234567890 2345678901 3456789012 4567890123 5678901234 6789012345 7890123456 socket_test\0";

static int _send_tcp4(int fd, uint32_t s_addr, uint32_t cycles)
{
	struct nfp_sockaddr_in addr = {0};
	uint32_t cycle = 0;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = s_addr;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	for (; cycle < cycles; cycle++)
		if (nfp_send(fd, tcp_buf, strlen(tcp_buf) + 1, 0) == -1) {
			NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
			return -1;
		}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_tcp4_local_ip(int fd)
{
	return _send_tcp4(fd, TEST_ADDR4, 1);
}

int send_multi_tcp4_any(int fd)
{
	return _send_tcp4(fd, NFP_INADDR_ANY, TCP_CYCLES);
}

int send_tcp4_any(int fd)
{
	return _send_tcp4(fd, NFP_INADDR_ANY, 1);
}

/* Send two tcp buffers with a timeout between. */
int send_tcp4_msg_waitall(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_send(fd, tcp_buf, strlen(tcp_buf), 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}

	sleep(1);

	if (nfp_send(fd, tcp_buf, strlen(tcp_buf), 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}


	NFP_INFO("SUCCESS.\n");
	return 0;
}


#ifdef INET6
int send_tcp6_local_ip(int fd)
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

	if (nfp_send(fd, tcp_buf, strlen(tcp_buf) + 1, 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_tcp6_any(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = nfp_in6addr_any;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_send(fd, tcp_buf, strlen(tcp_buf) + 1, 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /*INET6*/

static int _receive_tcp(int fd, uint32_t cycles)
{
	char buf[1024];
	nfp_size_t recv_size;
	nfp_ssize_t len;
	int fd_accepted = -1;
	uint32_t cycle = 0;

	recv_size = (cycles == 1) ? sizeof(buf) : strlen(tcp_buf) + 1;

	fd_accepted = nfp_accept(fd, NULL, NULL);

	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	for (; cycle < cycles; cycle++) {
		memset(buf, 0, recv_size);
		len = nfp_recv(fd_accepted, buf, recv_size, 0);
		if (len == -1) {
			NFP_ERR("FAILED to recv (errno = %d)\n",
				nfp_errno);
			nfp_close(fd_accepted);
			return -1;
		}
		buf[len] = 0;

		if (len != (nfp_ssize_t)(strlen(tcp_buf) + 1)) {
			NFP_ERR("FAILED : length received is wrong:[%d]\n",
				len);
			nfp_close(fd_accepted);
			return -1;
		}

		if (strcmp(buf, tcp_buf) != 0) {
			NFP_ERR("FAILED : data received is malformed:[%s]\n",
				buf);
			nfp_close(fd_accepted);
			return -1;
		}
		NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);
	}

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}


int receive_tcp(int fd)
{
	return _receive_tcp(fd, 1);
}

int receive_multi_tcp(int fd)
{
	return _receive_tcp(fd, TCP_CYCLES);
}

/* verify NFP_MSG_WAITALL works for nfp_recv. */
int receive_tcp4_msg_waitall(int fd)
{
	char buf[1024];
	nfp_size_t recv_size;
	nfp_ssize_t len, len2;
	int fd_accepted = -1;

	recv_size = strlen(tcp_buf);

	fd_accepted = nfp_accept(fd, NULL, NULL);

	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	memset(buf, 0, sizeof(buf));
#define SOME_DATA_LEN 5
	/* receive more than a tcp buffer but less than two. */
	len = nfp_recv(fd_accepted, buf, 2 * recv_size - SOME_DATA_LEN,
			NFP_MSG_WAITALL);
	if (len == -1) {
		NFP_ERR("FAILED to recv (errno = %d)\n",
			nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

#define EXTRA_FREE_SPACE 128
	/* receive what remains from the second tcp buffer. */
	len2 = nfp_recv(fd_accepted, buf + 2 * recv_size - SOME_DATA_LEN,
			SOME_DATA_LEN + EXTRA_FREE_SPACE, 0);
	if (len2 == -1) {
		NFP_ERR("FAILED to recv (errno = %d)\n",
			nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	/* Total length should be ok. */
	if (len + len2 != (nfp_ssize_t)(strlen(tcp_buf) * 2)) {
		NFP_ERR("FAILED : length received is wrong:[%d]\n", len + len2);
		nfp_close(fd_accepted);
		return -1;
	}

	/* First tcp buffer data is received. */
	if (strncmp(buf, tcp_buf, strlen(tcp_buf)) != 0) {
		NFP_ERR("FAILED : data received is malformed:[%s]\n", buf);
		nfp_close(fd_accepted);
		return -1;
	}

	/* Second tcp buffer data is received. */
	if (strcmp(buf +  strlen(tcp_buf), tcp_buf) != 0) {
		NFP_ERR("FAILED : data received is malformed:[%s]\n", buf
			+ strlen(tcp_buf));
		nfp_close(fd_accepted);
		return -1;
	}

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_receive(int fd)
{
	char buf[1024];
	int fd_accepted = -1;
	nfp_size_t recv_size = 1024;
	int len;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	len = nfp_recv(fd_accepted, buf, recv_size, 0);
	if (len == -1) {
		NFP_ERR("FAILED to recv (errno = %d)\n",
			nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}
	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
