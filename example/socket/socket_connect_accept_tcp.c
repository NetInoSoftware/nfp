/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_connect_accept_tcp.h"
#include "socket_util.h"

int init_tcp_bind_listen_local_ip(int *pfd_thread1, int *pfd_thread2)
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

	if (nfp_listen(*pfd_thread2, 10) == -1) {
		NFP_ERR("Failed to listen (errno = %d)\n", nfp_errno);
		return -1;
	}

	return 0;
}

int init_tcp_bind_listen_any(int *pfd_thread1, int *pfd_thread2)
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
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

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
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_listen(*pfd_thread2, 10) == -1) {
		NFP_ERR("Failed to listen (errno = %d)\n", nfp_errno);
		return -1;
	}

	return 0;
}

#ifdef INET6
int init_tcp6_bind_listen_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};
	int optval = 1;

	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	optval = 1;
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

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

	optval = 1;
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

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

	if (nfp_listen(*pfd_thread2, 10) == -1) {
		NFP_ERR("Failed to listen (errno = %d)\n", nfp_errno);
		return -1;
	}

	return 0;
}

int init_tcp6_bind_listen_any(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};
	int optval = 1;

	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	optval = 1;
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread1, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	addr.sin6_addr = nfp_in6addr_any;

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

	optval = 1;
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEADDR,
		&optval, sizeof(optval));
	nfp_setsockopt(*pfd_thread2, NFP_SOL_SOCKET, NFP_SO_REUSEPORT,
		&optval, sizeof(optval));

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = nfp_in6addr_any;

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_listen(*pfd_thread2, 10) == -1) {
		NFP_ERR("Failed to listen (errno = %d)\n", nfp_errno);
		return -1;
	}

	return 0;
}
#endif /* INET6 */

int connect_tcp4_local_ip(int fd)
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

int connect_tcp4_any(int fd)
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

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_tcp4(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);
	int fd_accepted = -1;

	fd_accepted = nfp_accept(fd, (struct nfp_sockaddr *)&addr,
			&addr_len);

	if (fd_accepted == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (addr_len != sizeof(struct nfp_sockaddr_in)) {
		NFP_ERR("Failed to accept: invalid address size %d\n",
			addr_len);
		return -1;
	}

	NFP_INFO("Address: 0x%x, port: %d.\n",
		odp_be_to_cpu_32(addr.sin_addr.s_addr),
		odp_be_to_cpu_16(addr.sin_port));

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("Failed to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_tcp4_null_addr(int fd)
{
	int fd_accepted;

	fd_accepted = nfp_accept(fd, NULL, NULL);

	if (fd_accepted == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("Failed to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}


#ifdef INET6
int connect_tcp6_local_ip(int fd)
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

int connect_tcp6_any(int fd)
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

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_tcp6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);
	int fd_accepted = -1;

	fd_accepted = nfp_accept(fd, (struct nfp_sockaddr *)&addr,
				&addr_len);
	if (fd_accepted == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (addr_len != sizeof(struct nfp_sockaddr_in6)) {
		NFP_ERR("Failed to accept: invalid address size %d\n",
			addr_len);
		return -1;
	}

	NFP_INFO("Address: %x:%x:%x:%x, port: %d.\n",
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[0]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[1]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[2]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[3]),
		odp_be_to_cpu_16(addr.sin6_port));

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("Failed to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_tcp6_null_addr(int fd)
{
	int fd_accepted = -1;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("Failed to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

