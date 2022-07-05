/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_create_close.h"

int create_close_udp(int fd)
{
	fd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_udp_noproto(int fd)
{
	fd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, 0);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_tcp(int fd)
{
	fd = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, NFP_IPPROTO_TCP);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_tcp_noproto(int fd)
{
	fd = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, 0);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int create_close_udp6(int fd)
{
	fd = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_udp6_noproto(int fd)
{
	fd = nfp_socket(NFP_AF_INET6, NFP_SOCK_DGRAM, 0);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_tcp6(int fd)
{
	fd = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, NFP_IPPROTO_TCP);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int create_close_tcp6_noproto(int fd)
{
	fd = nfp_socket(NFP_AF_INET6, NFP_SOCK_STREAM, 0);
	if (fd == -1) {
		NFP_ERR("Failed to create socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_close(fd) == -1) {
		NFP_ERR("Failed to close socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

