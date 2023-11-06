/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_getsockname.h"
#include "socket_util.h"

int getsockname_bind(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_sockaddr_in sin = {0};
	nfp_socklen_t sinlen = 0;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		     sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)", nfp_errno);
		return -1;
	}

	sinlen = sizeof(sin);
	if (nfp_getsockname(fd, (struct nfp_sockaddr *)&sin, &sinlen)) {
		NFP_ERR("Failed to get socket address: %d", nfp_errno);
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

int getsockname_not_bind(int fd)
{
	struct nfp_sockaddr_in sin = {0};
	nfp_socklen_t sinlen = 0;

	sinlen = sizeof(sin);
	if (nfp_getsockname(fd, (struct nfp_sockaddr *)&sin, &sinlen)) {
		NFP_ERR("Failed to get socket address: %d", nfp_errno);
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

#ifdef INET6
int getsockname_bind6(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	struct nfp_sockaddr_in6 sin6 = {0};
	nfp_socklen_t sin6len = 0;

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 2);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_bind(fd, (const struct nfp_sockaddr *)&addr,
		     sizeof(addr)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)", nfp_errno);
		return -1;
	}

	sin6len = sizeof(sin6);
	if (nfp_getsockname(fd, (struct nfp_sockaddr *)&sin6, &sin6len)) {
		NFP_ERR("Failed to get socket address: %d", nfp_errno);
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

int getsockname_not_bind6(int fd)
{
	struct nfp_sockaddr_in6 sin6 = {0};
	nfp_socklen_t sin6len = 0;

	sin6len = sizeof(sin6);
	if (nfp_getsockname(fd, (struct nfp_sockaddr *)&sin6, &sin6len)) {
		NFP_ERR("Failed to get socket address: %d", nfp_errno);
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
#endif /* INET6 */

