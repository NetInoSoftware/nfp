/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_getpeername.h"
#include "socket_util.h"

int getpeername_not_connect(int fd)
{
	struct nfp_sockaddr_in sin = {0};
	nfp_socklen_t sinlen = 0;

	sinlen = sizeof(sin);
	if (!nfp_getpeername(fd, (struct nfp_sockaddr *)&sin, &sinlen)) {
		NFP_ERR("Error: nfp_getsockname() should fail!");
		return -1;
	}

	if (nfp_errno != NFP_ENOTCONN) {
		NFP_ERR("Error: unexpected error: %d", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getpeername_connect(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_sockaddr_in sin = {0};
	nfp_socklen_t sinlen = 0;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_connect(fd, (const struct nfp_sockaddr *)&addr,
			sizeof(struct nfp_sockaddr_in))) {
		NFP_ERR("Failed to connect socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	sinlen = sizeof(sin);
	if (nfp_getpeername(fd, (struct nfp_sockaddr *)&sin, &sinlen)) {
		NFP_ERR("Failed to get peer address: %d", nfp_errno);
		return -1;
	}

	if (sinlen != sizeof(sin)) {
		NFP_ERR("Invalid address size: %d", sinlen);
		return -1;
	}

	NFP_INFO("Address: %s, port: %d.",
		 nfp_print_ip_addr(sin.sin_addr.s_addr),
		 odp_be_to_cpu_16(sin.sin_port));

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getpeername_accept(int fd)
{
	int fd_accept = -1;
	struct nfp_sockaddr_in sin = {0};
	nfp_socklen_t sinlen = 0;

	fd_accept = nfp_accept(fd, NULL, NULL);
	if (fd_accept == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	sinlen = sizeof(sin);
	if (nfp_getpeername(fd_accept, (struct nfp_sockaddr *)&sin, &sinlen)) {
		NFP_ERR("Failed to get peer address: %d", nfp_errno);
		return -1;
	}

	if (sinlen != sizeof(sin)) {
		NFP_ERR("Invalid address size: %d", sinlen);
		return -1;
	}

	NFP_INFO("Address: %s, port: %d.",
		 nfp_print_ip_addr(sin.sin_addr.s_addr),
		 odp_be_to_cpu_16(sin.sin_port));

	nfp_close(fd_accept);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int getpeername_not_connect6(int fd)
{
	struct nfp_sockaddr_in6 sin6 = {0};
	nfp_socklen_t sin6len = 0;

	sin6len = sizeof(sin6);
	if (!nfp_getpeername(fd, (struct nfp_sockaddr *)&sin6, &sin6len)) {
		NFP_ERR("Error: nfp_getsockname() should fail!");
		return -1;
	}

	if (nfp_errno != NFP_ENOTCONN) {
		NFP_ERR("Error: unexpected error: %d", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getpeername_connect6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	struct nfp_sockaddr_in6 sin6 = {0};
	nfp_socklen_t sin6len = 0;

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_connect(fd, (const struct nfp_sockaddr *)&addr, sizeof(addr))) {
		NFP_ERR("Failed to connect socket (errno = %d)\n", nfp_errno);
		return -1;
	}

	sin6len = sizeof(sin6);
	if (nfp_getpeername(fd, (struct nfp_sockaddr *)&sin6, &sin6len)) {
		NFP_ERR("Failed to get peer address: %d", nfp_errno);
		return -1;
	}

	if (sin6len != sizeof(sin6)) {
		NFP_ERR("Invalid address size: %d", sin6len);
		return -1;
	}

	NFP_INFO("Address: %s, port: %d.",
		 nfp_print_ip6_addr(sin6.sin6_addr.nfp_s6_addr),
		 odp_be_to_cpu_16(sin6.sin6_port));

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getpeername_accept6(int fd)
{
	int fd_accept = -1;
	struct nfp_sockaddr_in6 sin6 = {0};
	nfp_socklen_t sin6len = 0;

	fd_accept = nfp_accept(fd, NULL, NULL);
	if (fd_accept == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	sin6len = sizeof(sin6);
	if (nfp_getpeername(fd_accept, (struct nfp_sockaddr *)&sin6,
			    &sin6len)) {
		NFP_ERR("Failed to get peer address: %d", nfp_errno);
		return -1;
	}

	if (sin6len != sizeof(sin6)) {
		NFP_ERR("Invalid address size: %d", sin6len);
		return -1;
	}

	NFP_INFO("Address: %s, port: %d.",
		 nfp_print_ip6_addr(sin6.sin6_addr.nfp_s6_addr),
		 odp_be_to_cpu_16(sin6.sin6_port));

	nfp_close(fd_accept);
	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */
