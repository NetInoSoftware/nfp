/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_connect_udp.h"
#include "socket_util.h"

int connect_udp4(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 2);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int connect_bind_udp4(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 3);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_udp4(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 2);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_bind_udp4(int fd)
{
	struct nfp_sockaddr_in addr = {0};

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 3);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}


#ifdef INET6
int connect_udp6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 2);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_bind_udp6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 3);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_udp6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 2);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_bind_udp6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 3);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_udp6_any(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 2);
	addr.sin6_addr = nfp_in6addr_any;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_shutdown_bind_udp6_any(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = nfp_in6addr_any;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 3);
	addr.sin6_addr = nfp_in6addr_any;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */
