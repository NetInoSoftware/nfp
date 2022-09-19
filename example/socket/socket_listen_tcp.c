/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_listen_tcp.h"
#include "socket_util.h"

int init_tcp_bind_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};
	int optval = 1;

	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM,
				NFP_IPPROTO_TCP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	optval = 1;
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(*pfd_thread1, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM,
				NFP_IPPROTO_TCP);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create RCV socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	optval = 1;
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

#ifdef INET6
int init_tcp6_bind_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};

	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(*pfd_thread1, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}
#endif /* INET6 */

int listen_tcp(int fd)
{
	if (nfp_listen(fd, 10) == -1) {
		NFP_ERR("Failed to listen (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
