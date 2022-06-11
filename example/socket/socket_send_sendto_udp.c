/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */
#include "errno.h"
#include "nfp.h"
#include "socket_send_sendto_udp.h"
#include "socket_util.h"

int init_udp_bind_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};

	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				NFP_IPPROTO_UDP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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

	*pfd_thread2 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				NFP_IPPROTO_UDP);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create RCV socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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
int init_udp_bind_any(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};


	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
			NFP_IPPROTO_UDP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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

	*pfd_thread2 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
			NFP_IPPROTO_UDP);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create RCV socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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

	return 0;
}

#ifdef INET6
int init_udp6_bind_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};

	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	if (!nfp_parse_ip6_addr(TEST_ADDR6_STR, 0,
				addr.sin6_addr.nfp_s6_addr)) {
		NFP_ERR("Failed to convert IPv6 address\n");
		return -1;
	}

	if (nfp_bind(*pfd_thread1, (const struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	*pfd_thread2 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
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

int init_udp6_bind_any(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};

	*pfd_thread1 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create socket 1 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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

	*pfd_thread2 = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (*pfd_thread2 == -1) {
		NFP_ERR("Failed to create socket 2 (errno = %d)\n",
			nfp_errno);
		return -1;
	}

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

	return 0;
}
#endif /* INET6 */

int send_ip4_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	const char *buf = "socket_test";

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_send(fd, buf, strlen(buf), 0) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}
	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int sendto_ip4_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in dest_addr = {0};
	const char *buf = "socket_test";

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	dest_addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}
	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_ip4_udp_any(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	const char *buf = "socket_test";

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_send(fd, buf, strlen(buf), 0) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int sendto_ip4_udp_any(int fd)
{
	struct nfp_sockaddr_in dest_addr = {0};
	const char *buf = "socket_test";

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	dest_addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int send_ip6_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	const char *buf = "socket_test";

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}
	if (nfp_send(fd, buf, strlen(buf), 0) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
int sendto_ip6_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "socket_snd2";

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, dest_addr.sin6_addr.nfp_s6_addr);

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_ip6_udp_any(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	const char *buf = "socket_test";

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = nfp_in6addr_any;

	if ((nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in6)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	if (nfp_send(fd, buf, strlen(buf), 0) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int sendto_ip6_udp_any(int fd)
{
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "socket_test";

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT);
	dest_addr.sin6_addr = nfp_in6addr_any;

	if (nfp_sendto(fd, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&dest_addr,
		sizeof(dest_addr)) != (nfp_ssize_t)strlen(buf)) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("Data (%s) sent successfully.\n", buf);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */
