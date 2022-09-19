/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_IFADDRS_H__
#define __NFP_IFADDRS_H__

#include "nfp_socket.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

struct nfp_ifaddrs {
	struct nfp_ifaddrs  *ifa_next;    /* Next item in list */
	char                *ifa_name;    /* Name of interface */
	unsigned int         ifa_flags;   /* Flags from SIOCGIFFLAGS */
	struct nfp_sockaddr *ifa_addr;    /* Address of interface */
	struct nfp_sockaddr *ifa_netmask; /* Netmask of interface */
	union {
		struct nfp_sockaddr *ifu_broadaddr;
					/* Broadcast address of interface */
		struct nfp_sockaddr *ifu_dstaddr;
					/* Point-to-point destination address */
	} ifa_ifu;
#define              nfp_ifa_broadaddr ifa_ifu.ifu_broadaddr
#define              nfp_ifa_dstaddr   ifa_ifu.ifu_dstaddr
	void                *ifa_data;    /* Address-specific data */
};

int nfp_getifaddrs(struct nfp_ifaddrs **ifap);

void nfp_freeifaddrs(struct nfp_ifaddrs *ifa);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IFADDRS_H__ */
