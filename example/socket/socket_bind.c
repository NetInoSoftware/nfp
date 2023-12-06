/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_bind.h"
#include "socket_util.h"

int init_udp_create_socket(int *pfd_thread1, int *pfd_thread2)
{
	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

int init_tcp_create_socket(int *pfd_thread1, int *pfd_thread2)
{
	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

#ifdef INET6
int init_udp6_create_socket(int *pfd_thread1, int *pfd_thread2)
{
	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

int init_tcp6_create_socket(int *pfd_thread1, int *pfd_thread2)
{
	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}
#endif /* INET6 */


int bind_ip4_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int bind_ip4_any(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int bind_ip6_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int bind_ip6_any(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = nfp_in6addr_any;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

