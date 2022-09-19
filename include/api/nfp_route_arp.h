/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_ROUTE_ARP_H__
#define __NFP_ROUTE_ARP_H__

#include <stdint.h>
#include <string.h>
#include "nfp_log.h"
#include "nfp_print.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* ROUTE: ADD/DEL*/

struct nfp_route_msg {
	uint32_t type;
#define NFP_ROUTE_ADD		1
#define NFP_ROUTE_DEL		2
#define NFP_MOBILE_ROUTE_ADD	3
#define NFP_MOBILE_ROUTE_DEL	4
#define NFP_LOCAL_INTERFACE_ADD	5
#define NFP_LOCAL_INTERFACE_DEL	6
#define NFP_ROUTE6_ADD		7
#define NFP_ROUTE6_DEL		8
	uint32_t flags;
#define	NFP_RTF_NET		0x1	/* route usable */
#define	NFP_RTF_GATEWAY		0x2	/* destination is a gateway */
#define	NFP_RTF_HOST		0x4	/* host entry (lb, p2p)(net otherwise)*/
#define	NFP_RTF_REJECT		0x8	/* host or net unreachable */
#define	NFP_RTF_BLACKHOLE	0x1000	/* just discard pkts (during updates) */
#define	NFP_RTF_LOCAL		0x200000/* route represents a local address */
#define	NFP_RTF_BROADCAST	0x400000/* route represents a bcast address */
#define	NFP_RTF_MULTICAST	0x800000/* route represents a mcast address */
	uint32_t dst;
	uint32_t masklen;
	uint32_t gw;
	uint32_t port;
	uint16_t vlan;
	uint16_t vrf;
	uint8_t  dst6[16];
	uint8_t  gw6[16];
};

int32_t nfp_set_route_msg(struct nfp_route_msg *msg);

/**
 * Set or delete an IPv4 route
 *
 * @param type	Operation type:  NFP_ROUTE_ADD, NFP_ROUTE_DEL
 * @param vrf	VRF for the route
 * @param port	Interface port
 * @param vlan	Interface subport
 * @param dst	Destination address
 * @param masklen	Destination address mask length
 * @param gw	Gateway address
 * @param flags	Route RTF flags
 *
 * @retval 0	On success
 * @retval <0	On failure
 */
static inline int32_t nfp_set_route(uint32_t type, uint16_t vrf,
				    uint32_t port, uint16_t vlan,
				    uint32_t dst, uint32_t masklen,
				    uint32_t gw, uint32_t flags)
{
	struct nfp_route_msg msg;

#if defined(NFP_DEBUG)
	if (type == NFP_ROUTE6_ADD || type == NFP_ROUTE6_DEL) {
		NFP_ERR("Incompatible type=%d\n", type);
		return -1;
	}
#endif
	msg.type    = type;
	msg.flags   = flags;
	msg.vrf     = vrf;
	msg.vlan    = vlan;
	msg.port    = port;

	msg.dst     = dst;
	msg.masklen = masklen;
	msg.gw      = gw;

	return nfp_set_route_msg(&msg);
}

/**
 * Set or delete an IPv6 route
 *
 * @param type	Operation type:  NFP_ROUTE6_ADD, NFP_ROUTE6_DEL
 * @param vrf	VRF for the route (not supported)
 * @param port	Interface port
 * @param vlan	Interface subport
 * @param dst6	Destination address
 * @param masklen	Destination address mask length
 * @param gw6	Gateway address
 * @param flags	Route RTF flags
 *
 * @retval 0	On success
 * @retval <0	On failure
 */
static inline int32_t nfp_set_route6(uint32_t type, uint16_t vrf,
				     uint32_t port, uint16_t vlan,
				     const uint8_t dst6[],
				     uint32_t masklen,
				     const uint8_t gw6[],
				     uint32_t flags)
{
	struct nfp_route_msg msg;

#if defined(NFP_DEBUG)
	if (type != NFP_ROUTE6_ADD && type != NFP_ROUTE6_DEL) {
		NFP_ERR("Incompatible type=%d\n", type);
		return -1;
	}
#endif
	msg.type    = type;
	msg.flags   = flags;
	msg.vrf     = vrf;
	msg.vlan    = vlan;
	msg.port    = port;

	if (dst6) {
		memcpy(msg.dst6, dst6,
		       (masklen > 0) ? (1 + ((masklen - 1) >> 3)) : 0);
	}
	msg.masklen = masklen;
	if (gw6) {
		memcpy(msg.gw6, gw6, 16);
	}

	return nfp_set_route_msg(&msg);
}

/* ROUTE: SHOW */

#define NFP_SHOW_ARP        0
#define NFP_SHOW_ROUTES     1
/**
 * Shows the list of routes or ARP entries
 *
 * @param pr	NFP text printer
 * @param what	What to display: NFP_SHOW_ARP or NFP_SHOW_ROUTES
 */
void nfp_show_routes(nfp_print_t *pr, int what);

/* ROUTE operations */
struct nfp_nh_entry *nfp_get_next_hop(uint16_t vrf,
		uint32_t addr, uint32_t *flags);
struct nfp_nh6_entry *nfp_get_next_hop6(uint16_t vrf,
		uint8_t *addr, uint32_t *flags);

/* ROUTE default */
/**
 * Set or delete an IPv4 default route
 *
 * @param type	Operation type:  NFP_ROUTE_ADD, NFP_ROUTE_DEL
 * @param vrf	VRF for the route
 * @param port	Interface port
 * @param vlan	Interface subport
 * @param gw	Gateway address
 * @param flags	Route RTF flags
 *
 * @retval 0	On success
 * @retval <0	On failure
 */
int32_t nfp_set_route_dflt(uint32_t type, uint16_t vrf,
			   uint32_t port, uint16_t vlan,
			   uint32_t gw, uint32_t flags);

/**
 * Set or delete the IPv6 default route
 *
 * @param type	Operation type:  NFP_ROUTE6_ADD, NFP_ROUTE6_DEL
 * @param vrf	VRF for the route (not supported)
 * @param port	Interface port
 * @param vlan	Interface subport
 * @param gw6	Gateway address
 * @param flags	Route RTF flags
 *
 * @retval 0	On success
 * @retval <0	On failure
 */
int32_t nfp_set_route6_dflt(uint32_t type, uint16_t vrf,
			    uint32_t port, uint16_t vlan,
			    const uint8_t gw6[], uint32_t flags);

/* ARP */
struct nfp_ifnet;
int nfp_add_mac(struct nfp_ifnet *dev, uint32_t addr, uint8_t *mac);
int nfp_get_mac(struct nfp_ifnet *dev, struct nfp_nh_entry *nh_data,
		uint32_t addr, uint32_t is_link_local, uint8_t *mac_out);
void nfp_add_mac6(struct nfp_ifnet *dev, uint8_t *addr, uint8_t *mac);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ROUTE_ARP_H__ */
