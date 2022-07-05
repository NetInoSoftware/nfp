/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_send_recv_udp.h"
#include "socket_util.h"

int init_udp_local_ip(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};


	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				NFP_IPPROTO_UDP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
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

int init_udp_any(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};


	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
			NFP_IPPROTO_UDP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
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

int init_udp_loopback(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in addr = {0};

	*pfd_thread1 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				  NFP_IPPROTO_UDP);
	if (*pfd_thread1 == -1) {
		NFP_ERR("Failed to create SEND socket (errno = %d)\n",
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
	addr.sin_addr.s_addr = TEST_LOOP4;

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		     sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

int send_udp_local_ip(int fd)
{
	const char *buf = "socket_test";
	struct nfp_sockaddr_in dest_addr = {0};

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

int send_udp_loopback(int fd)
{
	const char *buf = "socket_test";
	struct nfp_sockaddr_in dest_addr = {0};

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	dest_addr.sin_addr.s_addr = TEST_LOOP4;

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

int recv_udp(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_udp_any(int fd)
{
	const char *buf = "socket_test";
	struct nfp_sockaddr_in dest_addr = {0};

	dest_addr.sin_len = sizeof(struct nfp_sockaddr_in);
	dest_addr.sin_family = NFP_AF_INET;
	dest_addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	dest_addr.sin_addr.s_addr = NFP_INADDR_ANY;

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

int recvfrom_udp(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_sockaddr_in addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);

	len = nfp_recvfrom(fd, buf, buf_len, 0,
			   (struct nfp_sockaddr *)&addr, &addr_len);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	if (addr_len != sizeof(addr)) {
		NFP_ERR("Failed to rcv source address: %d (errno = %d)\n",
			addr_len, nfp_errno);
		return -1;
	}

	NFP_INFO("Data was received on address 0x%x, port = %d.\n",
		odp_be_to_cpu_32(addr.sin_addr.s_addr),
		odp_be_to_cpu_16(addr.sin_port));
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int recvfrom_udp_null_addr(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;

	len = nfp_recvfrom(fd, buf, buf_len, 0, NULL, NULL);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int init_udp6_loopback(int *pfd_thread1, int *pfd_thread2)
{
	struct nfp_sockaddr_in6 addr = {0};

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

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin6_addr = TEST_LOOP6;

	if (nfp_bind(*pfd_thread2, (const struct nfp_sockaddr *)&addr,
		     sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	return 0;
}

int send_udp6_local_ip(int fd)
{
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "socket_snd2";

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

int send_udp6_any(int fd)
{
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "socket_test";

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	dest_addr.sin6_addr = nfp_in6addr_any;

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

int send_udp6_loopback(int fd)
{
	struct nfp_sockaddr_in6 dest_addr = {0};
	const char *buf = "socket_test";

	dest_addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	dest_addr.sin6_family = NFP_AF_INET6;
	dest_addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	dest_addr.sin6_addr = TEST_LOOP6;

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

int recvfrom_udp6(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_sockaddr_in6 addr = {0};
	nfp_socklen_t addr_len = sizeof(addr);

	len = nfp_recvfrom(fd, buf, buf_len, 0,
			   (struct nfp_sockaddr *)&addr, &addr_len);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	if (addr_len != sizeof(addr)) {
		NFP_ERR("Failed to rcv source address: %d (errno = %d)\n",
			addr_len, nfp_errno);
		return -1;
	}

	NFP_INFO("Data was received on address %x:%x:%x:%x, port = %d.\n",
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[0]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[1]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[2]),
		odp_be_to_cpu_32(addr.sin6_addr.nfp_s6_addr32[3]),
		odp_be_to_cpu_16(addr.sin6_port));
	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /*INET6*/
