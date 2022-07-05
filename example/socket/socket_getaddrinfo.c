/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp_socket_types.h"
#include "nfp_socket.h"
#include "nfp_netdb.h"
#include "nfp_log.h"
#include "nfp_in.h"
#include "nfp_in6.h"
#include "nfp_utils.h"
#include "socket_getaddrinfo.h"

const char * const ai_errcodes[] = {
	"Success",
	"NFP_EAI_ADDRFAMILY",
	"NFP_EAI_AGAIN",
	"NFP_EAI_BADFLAGS",
	"NFP_EAI_FAIL",
	"NFP_EAI_FAMILY",
	"NFP_EAI_MEMORY",
	"NFP_EAI_NODATA",
	"NFP_EAI_NONAME",
	"NFP_EAI_SERVICE",
	"NFP_EAI_SOCKTYPE",
	"NFP_EAI_SYSTEM",
	"NFP_EAI_BADHINTS",
	"NFP_EAI_PROTOCOL",
	"NFP_EAI_OVERFLOW"
};

static const char *get_family_name(int ai_family)
{
	if (ai_family == NFP_AF_INET)
		return "NFP_AF_INET";
	else if (ai_family == NFP_AF_INET6)
		return "NFP_AF_INET6";
	else if (ai_family == NFP_AF_UNSPEC)
		return "NFP_AF_UNSPEC";

	return "Other";
}

static const char *get_socket_type(int ai_socktype)
{
	switch (ai_socktype) {
	case NFP_SOCK_STREAM:
		return "NFP_SOCK_STREAM";
	case NFP_SOCK_DGRAM:
		return "NFP_SOCK_DGRAM";
	case NFP_SOCK_RAW:
		return "NFP_SOCK_RAW";
	case NFP_SOCK_SEQPACKET:
		return "NFP_SOCK_SEQPACKET";
	};
	return "Other";
}

static const char *get_protocol(int ai_protocol)
{
	switch (ai_protocol) {
	case NFP_IPPROTO_UDP:
		return "NFP_IPPROTO_UDP";
	case NFP_IPPROTO_TCP:
		return "NFP_IPPROTO_TCP";
	case NFP_IPPROTO_SCTP:
		return "NFP_IPPROTO_SCTP";
	case NFP_IPPROTO_UDPLITE:
		return "NFP_IPPROTO_UDPLITE";
	}
	return "Other";
}

static void addrinfo_print(struct nfp_addrinfo *res)
{
	struct nfp_sockaddr_in *sin;
	struct nfp_sockaddr_in6 *sin6;

	if (!res) {
		NFP_INFO("addrinfo: Empty!!");
		return;
	}

	while (res) {
		NFP_INFO("\naddrinfo:\n"
			 "\tai_flags: 0x%x\n"
			 "\tai_family: %s (%d)\n"
			 "\tai_socktype: %s (%d)\n"
			 "\tai_protocol: %s (%d)\n"
			 "\tai_canonname: %s\n"
			 "\tai_addrlen: %d\n",
			 res->ai_flags,
			 get_family_name(res->ai_family), res->ai_family,
			 get_socket_type(res->ai_socktype), res->ai_socktype,
			 get_protocol(res->ai_protocol), res->ai_protocol,
			 res->ai_canonname ? res->ai_canonname : "null",
			 res->ai_addrlen);

		if (!res->ai_addr) {
			NFP_INFO("\n\taddr: null\n");
			goto next;
		}

		if (res->ai_family != res->ai_addr->sa_family)
			NFP_INFO("\tWarning: addr family inconsistent\n");

		if (res->ai_addr->sa_family == NFP_AF_INET) {
			sin = (struct nfp_sockaddr_in *)res->ai_addr;

			NFP_INFO("\n\taddr:\n"
				 "\t\tsin_family: %s\n"
				 "\t\tsin_port: %d\n"
				 "\t\tsin_len = %d\n"
				 "\t\tsin_addr = %s\n",
				 get_family_name(sin->sin_family),
				 odp_be_to_cpu_16(sin->sin_port),
				 sin->sin_len,
				 nfp_print_ip_addr(sin->sin_addr.s_addr));
		} else if (res->ai_addr->sa_family == NFP_AF_INET6) {
			struct nfp_in6_addr	*sin6_addr;

			sin6 = (struct nfp_sockaddr_in6 *)res->ai_addr;
			sin6_addr = &sin6->sin6_addr;

			NFP_INFO("\n\taddr:\n"
				 "\t\tsin6_family: %s\n"
				 "\t\tsin6_port: %d\n"
				 "\t\tsin6_len = %d\n"
				 "\t\tsin6_flowinfo = %d\n"
				 "\t\tsin6_scope_id = %d\n"
				 "\t\tsin6_addr = %s\n",
				 get_family_name(sin6->sin6_family),
				 odp_be_to_cpu_16(sin6->sin6_port),
				 sin6->sin6_len,
				 sin6->sin6_flowinfo,
				 sin6->sin6_scope_id,
				 nfp_print_ip6_addr(sin6_addr->nfp_s6_addr));
		} else {
			NFP_INFO("\n\taddr: other\n");
		}

next:
		res = res->ai_next;
	}
}

int gai_strerror_print(int fd)
{
	int i;

	(void)fd;

	for (i = 0; i < NFP_EAI_MAX; i++)
		NFP_INFO("Code %s (%d): %s", ai_errcodes[i], i,
			 nfp_gai_strerror(i));

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_null_hints_null_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;

	(void)fd;

	error = nfp_getaddrinfo(NULL, "2048", NULL, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_null_hints_num_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;

	(void)fd;

	error = nfp_getaddrinfo("192.168.100.200", "2048", NULL, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_null_hints_num6_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;

	(void)fd;

	error = nfp_getaddrinfo("2001:1baf::2", "2048", NULL, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ipv4_hints_null_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_INET;
	hints.ai_socktype = NFP_SOCK_DGRAM;
	hints.ai_flags = 0;

	error = nfp_getaddrinfo(NULL, "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ipv6_hints_null_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_INET6;
	hints.ai_protocol = NFP_IPPROTO_TCP;
	hints.ai_flags = 0;

	error = nfp_getaddrinfo(NULL, "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ai_passive_hints_null_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_PASSIVE;

	error = nfp_getaddrinfo(NULL, "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ai_canonname_hints_num_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_CANONNAME;

	error = nfp_getaddrinfo("192.168.100.150", "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ipv6_ai_v4mapped_hints_num_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_INET6;
	hints.ai_protocol = NFP_IPPROTO_UDP;
	hints.ai_flags = NFP_AI_V4MAPPED;

	error = nfp_getaddrinfo("192.168.100.150", "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ai_numerichost_hints_num_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_NUMERICHOST;

	error = nfp_getaddrinfo("192.168.100.150", "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_ai_addrconfig_hints_num_host(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_ADDRCONFIG;

	error = nfp_getaddrinfo("192.168.100.150", "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_localhost(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_CANONNAME;
	hints.ai_protocol = NFP_IPPROTO_UDP;

	error = nfp_getaddrinfo("localhost", "2048", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getaddrinfo_localhost_service(int fd)
{
	int error = 0;
	struct nfp_addrinfo *res = NULL;
	struct nfp_addrinfo hints = {0};

	(void)fd;

	hints.ai_family = NFP_AF_UNSPEC;
	hints.ai_flags = NFP_AI_CANONNAME;
	hints.ai_protocol = NFP_IPPROTO_TCP;

	error = nfp_getaddrinfo("localhost", "www", &hints, &res);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}

	addrinfo_print(res);

	nfp_freeaddrinfo(res);
	NFP_INFO("SUCCESS.\n");
	return 0;
}
