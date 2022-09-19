/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp_netdb.h"
#include "nfp_log.h"
#include "nfp_utils.h"
#include "socket_getservent.h"

static void servent_print(const struct nfp_servent *sp)
{
	const char **alss;

	if (!sp) {
		NFP_INFO("servent: empty");
		return;
	}

	NFP_INFO("\nservent:\n"
		 "\ts_name = %s\n"
		 "\ts_port = %d\n"
		 "\ts_proto = %s\n",
		 sp->s_name ? sp->s_name : "null",
		 odp_be_to_cpu_16(sp->s_port),
		  sp->s_proto ? sp->s_proto : "null");

	for (alss = sp->s_aliases; *alss; alss++)
		NFP_INFO("\n\t\talias: %s\n", *alss);
}

int getservbyname_null_type(int fd)
{
	const struct nfp_servent *sp = NULL;

	(void)fd;

	sp = nfp_getservbyname("kerberos", NULL);
	if (!sp) {
		NFP_INFO("FAILED: failed to get the service");
		return 0;
	}

	servent_print(sp);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getservbyname_type(int fd)
{
	const struct nfp_servent *sp = NULL;

	(void)fd;

	sp = nfp_getservbyname("kerberos", "udp");
	if (!sp) {
		NFP_INFO("FAILED: failed to get the service");
		return 0;
	}

	servent_print(sp);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getservbyport_null_type(int fd)
{
	const struct nfp_servent *sp = NULL;

	(void)fd;

	sp = nfp_getservbyport(odp_cpu_to_be_16(49), NULL);
	if (!sp) {
		NFP_INFO("FAILED: failed to get the service");
		return 0;
	}

	servent_print(sp);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int getservbyport_type(int fd)
{
	const struct nfp_servent *sp = NULL;

	(void)fd;

	sp = nfp_getservbyport(odp_cpu_to_be_16(88), "udp");
	if (!sp) {
		NFP_INFO("FAILED: failed to get the service");
		return 0;
	}

	servent_print(sp);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

