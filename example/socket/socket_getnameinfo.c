/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp_netdb.h"
#include "nfp_in.h"
#include "nfp_in6.h"
#include "nfp_log.h"
#include "nfp_utils.h"
#include "socket_getnameinfo.h"
#include "socket_util.h"

int getnameinfo_ipv4_service_only(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in sin = {0};
	char serv[NFP_NI_MAXSERV];

	(void)fd;

	sin.sin_len = sizeof(sin);
	sin.sin_family = NFP_AF_INET;
	sin.sin_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip_addr("192.168.35.45", &sin.sin_addr.s_addr)) {
		NFP_ERR("FAILED: Failed to parse IP address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin, sizeof(sin),
				NULL, 0, serv, sizeof(serv), 0);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service: %s", serv);

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin, sizeof(sin),
				NULL, 0, serv, sizeof(serv), NFP_NI_DGRAM);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service (NFP_NI_DGARM): %s", serv);

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin, sizeof(sin),
				NULL, 0, serv, sizeof(serv),
				NFP_NI_NUMERICSERV);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service (NFP_NI_NUMERICSERV): %s", serv);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int getnameinfo_ipv6_service_only(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in6 sin6 = {0};
	char serv[NFP_NI_MAXSERV];

	(void)fd;

	sin6.sin6_len = sizeof(sin6);
	sin6.sin6_family = NFP_AF_INET6;
	sin6.sin6_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip6_addr(TEST_ADDR6_STR, 0,
				sin6.sin6_addr.nfp_s6_addr)) {
		NFP_ERR("FAILED: Failed to parse IP6 address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin6,
				sizeof(sin6),
				NULL, 0, serv, sizeof(serv), 0);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service: %s", serv);

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin6,
				sizeof(sin6),
				NULL, 0, serv, sizeof(serv), NFP_NI_DGRAM);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service (NFP_NI_DGARM): %s", serv);

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin6,
				sizeof(sin6),
				NULL, 0, serv, sizeof(serv),
				NFP_NI_NUMERICSERV);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Service (NFP_NI_NUMERICSERV): %s", serv);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

int getnameinfo_ipv4_host_numeric_only(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in sin = {0};
	char host[NFP_NI_MAXHOST];

	(void)fd;

	sin.sin_len = sizeof(sin);
	sin.sin_family = NFP_AF_INET;
	sin.sin_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip_addr("192.168.35.45", &sin.sin_addr.s_addr)) {
		NFP_ERR("FAILED: Failed to parse IP address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin, sizeof(sin),
				host, sizeof(host), NULL, 0,
				NFP_NI_NUMERICHOST);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Host: %s", host);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int getnameinfo_ipv6_host_numeric_only(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in6 sin6 = {0};
	char host[NFP_NI_MAXHOST];

	(void)fd;

	sin6.sin6_len = sizeof(sin6);
	sin6.sin6_family = NFP_AF_INET6;
	sin6.sin6_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip6_addr(TEST_ADDR6_STR, 0,
				sin6.sin6_addr.nfp_s6_addr)) {
		NFP_ERR("FAILED: Failed to parse IP6 address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin6,
				sizeof(sin6),
				host, sizeof(host), NULL, 0,
				NFP_NI_NUMERICHOST);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Host: %s", host);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

int getnameinfo_ipv4_host_numeric_service(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in sin = {0};
	char host[NFP_NI_MAXHOST];
	char serv[NFP_NI_MAXSERV];

	(void)fd;

	sin.sin_len = sizeof(sin);
	sin.sin_family = NFP_AF_INET;
	sin.sin_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip_addr("192.168.35.45", &sin.sin_addr.s_addr)) {
		NFP_ERR("FAILED: Failed to parse IP address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin, sizeof(sin),
				host, sizeof(host), serv, sizeof(serv),
				NFP_NI_NUMERICHOST);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Host: %s", host);
	NFP_INFO("Service: %s", serv);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int getnameinfo_ipv6_host_numeric_service(int fd)
{
	int error = 0;
	struct nfp_sockaddr_in6 sin6 = {0};
	char host[NFP_NI_MAXHOST];
	char serv[NFP_NI_MAXSERV];

	(void)fd;

	sin6.sin6_len = sizeof(sin6);
	sin6.sin6_family = NFP_AF_INET6;
	sin6.sin6_port = odp_cpu_to_be_16(512);
	if (!nfp_parse_ip6_addr(TEST_ADDR6_STR, 0,
				sin6.sin6_addr.nfp_s6_addr)) {
		NFP_ERR("FAILED: Failed to parse IP6 address");
		return 0;
	}

	error = nfp_getnameinfo((const struct nfp_sockaddr *)&sin6,
				sizeof(sin6),
				host, sizeof(host), serv, sizeof(serv),
				NFP_NI_NUMERICHOST);
	if (error) {
		NFP_INFO("FAILED: %s : %s\n", ai_errcodes[error],
			 nfp_gai_strerror(error));
		return 0;
	}
	NFP_INFO("Host: %s", host);
	NFP_INFO("Service: %s", serv);

	NFP_INFO("SUCCESS.\n");
	return 0;
}
#endif /* INET6 */

