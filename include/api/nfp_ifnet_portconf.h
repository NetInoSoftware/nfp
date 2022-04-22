/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_PORTCONF_H__
#define __NFP_PORTCONF_H__

#include <odp_api.h>
#include "nfp_print.h"
#include "nfp_config.h"
#include "nfp_ifnet.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/**
 * NFP interface ports
 *
 * The organization of NFP interfaces is based on the concept of interface port.
 * A port represents either a network interface (physical or otherwise) or a
 * pseudo-port used to organize pseudo-interfaces e.g. gre, local, vxlan, etc.
 *
 * Current array of ports:
 * [0 - NFP_FP_INTERFACE_MAX - 1]: network interfaces
 * NFP_IFPORT_LOCAL              : local pseudo-interfaces
 * NFP_IFPORT_VXLAN              : vxlan pseudo-interfaces
 * NFP_IFPORT_GRE                : gre pseudo-interfaces
 *
 * Each port may have a number of sub-ports. The tuple (port, sub-port) is
 * uniquely identifying an NFP interface.
 *
 * The meaning of 'sub-port' depends on the type of port:
 * NFP_IFPORT_NET_XXX:
 *  - sub-port NFP_IFPORT_NET_SUBPORT_ITF represents the network interface
 *  - sub-port != NFP_IFPORT_NET_SUBPORT_ITF represents vlan
 * NFP_IFPORT_LOCAL:
 *  -sub-ports represents local interface ID
 * NFP_IFPORT_VXLAN:
 *  -sub-ports represents VXLAN network identifier (VNI)
 * NFP_IFPORT_GRE:
 *  -sub-ports represents GRE ID
 **/

enum {
	NFP_IFPORT_NET_FIRST = 0,
	NFP_IFPORT_NET_LAST = NFP_FP_INTERFACE_MAX - 1,
	NFP_IFPORT_LOCAL,
	NFP_IFPORT_VXLAN,
	NFP_IFPORT_GRE,
	NFP_IFPORT_NUM
};

#define NFP_IFPORT_IS_NET_U(_port) \
	(_port <= NFP_IFPORT_NET_LAST)

#define NFP_IFPORT_IS_NET(_port) \
	(_port >= NFP_IFPORT_NET_FIRST && NFP_IFPORT_IS_NET_U(_port))

#define NFP_IFPORT_IS_LOCAL(_port) \
	(_port == NFP_IFPORT_LOCAL)

#define NFP_IFPORT_IS_VXLAN(_port) \
	(_port == NFP_IFPORT_VXLAN)

#define NFP_IFPORT_IS_GRE(_port) \
	(_port == NFP_IFPORT_GRE)

/* Sub-port of the network (physical or otherwise) interface */
#define NFP_IFPORT_NET_SUBPORT_ITF 4096

/**
 * Create an NFP network interface port (ifport)
 *
 * Open an ODP interface using its name and configuration parameters.
 * The coresponding NFP interface is created and is uniquely identified
 * by the tuple (port, sub-port). Sub-port has always the value
 * NFP_IFPORT_NET_SUBPORT_ITF.
 *
 * This function can be used anytime to open ODP interfaces that were not opened
 * during nfp_initialize(). One can specify no interface in nfp_initialize()
 * and open one by one using this functionality.
 *
 * @param if_name Interface name to open
 * @param pktio_param Specify packet access mode for this
 *        interface
 * @param pktin_param Specify packet input queue parameters for
 *        this interface
 * @param pktout_param Specify packet output queue parameters for
 *        this interface
 * @param if_sp_mgmt Enable slow path interface management.
 * @param port Get the port value of the created interface.
 * @param subport Get the sub-port value of the created interface.
 *        The value is always NFP_IFPORT_NET_SUBPORT_ITF.
 *
 * @retval 0 on success
 * @retval -1 on failure
 *
 * @see nfp_initialize() can init interfaces.
 */
int nfp_ifport_net_create(char *if_name, odp_pktio_param_t *pktio_param,
			  odp_pktin_queue_param_t *pktin_param,
			  odp_pktout_queue_param_t *pktout_param,
			  odp_bool_t if_sp_mgmt,
			  int *port, uint16_t *subport);

/**
 * Get NFP interface associated with the network port
 *
 * Network port is searched by port ID. The function is equivalent with
 * calling nfp_ifport_ifnet_get(port, NFP_IFPORT_NET_SUBPORT_ITF).
 *
 * @param port Specify the network port
 *
 * @retval NFP interface on success
 * @retval NFP_IFNET_INVALID on failure
 */
nfp_ifnet_t nfp_ifport_net_ifnet_get_by_port(int port);

/**
 * Get NFP interface associated with the network port
 *
 * Network port is searched by ODP name.
 *
 * @param if_name Specify the interface ODP name
 *
 * @retval NFP interface on success
 * @retval NFP_IFNET_INVALID on failure
 */
nfp_ifnet_t nfp_ifport_net_ifnet_get_by_name(char *if_name);

/**
 * Get ODP pktio associated with the network port
 *
 * The function is equivalent with getting network port interface and
 * and retrieving ODP pktio.
 *
 * @param port Specify the network port
 *
 * @retval ODP pktio on success
 * @retval ODP_PKTIO_INVALID on failure
 */
odp_pktio_t nfp_ifport_net_pktio_get(int port);

/**
 * Get slow path ODP queue associated with the network port
 *
 * The function is equivalent with getting network port interface and
 * and retrieving the slow path queue.
 *
 * @retval slow path ODP queue on success
 * @retval ODP_QUEUE_INVALID on failure
 */
odp_queue_t nfp_ifport_net_spq_get(int port);

/**
 * Get loopback ODP queue associated with the network port
 *
 * The function is equivalent with getting network port interface and
 * and retrieving the loopback queue.
 *
 * @retval loopback ODP queue on success
 * @retval ODP_QUEUE_INVALID on failure
 */
odp_queue_t nfp_ifport_net_loopq_get(int port);

/**
 * Get NFP interface associate with a tuple (port, sub-port)
 *
 * @param port Specify the network port
 * @param subport Specify the network sub-port
 *
 * @retval NFP interface on success
 * @retval NFP_IFNET_INVALID on failure
 */
nfp_ifnet_t nfp_ifport_ifnet_get(int port, uint16_t subport);

/**
 * Get total number of ports defined
 *
 * The value includes used and not used network ports and all
 * pseudo-ports
 *
 * @retval number of ports defined
 */
int nfp_ifport_count(void);

/* Interfaces: UP/DOWN */

/**
 * Configure IPv4 address on a network port interface
 *
 * The function applies to the interface associated to the port (when
 * subport_vlan is NFP_IFPORT_NET_SUBPORT_ITF) or to a VLAN.
 * VLAN interface is created if it does not exists.
 *
 * @param port Specify the network port
 * @param subport_vlan Specify the network sub-port.
 * @param vrf Virtual routing table
 * @param addr IPv4 address to set
 * @param masklen Mask length
 * @param if_sp_mgmt Slow path interface management for the newly
 * created VLAN interface
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_net_ipv4_up(int port, uint16_t subport_vlan,
				   uint16_t vrf, uint32_t addr, int masklen,
				   odp_bool_t if_sp_mgmt);

/**
 * Configure IPv4 address on a network port interface with dhcp
 *
 * The function applies to the interface associated to the port (when
 * subport_vlan is NFP_IFPORT_NET_SUBPORT_ITF) or to a VLAN.
 * VLAN interface is created if it does not exists.
 *
 * @param port Specify the network port
 * @param subport_vlan Specify the network sub-port.
 * @param vrf Virtual routing table
 * @param if_sp_mgmt Slow path interface management for the newly
 * created VLAN interface
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_net_dhcp_up(int port, uint16_t subport_vlan,
				   uint16_t vrf, odp_bool_t if_sp_mgmt);

/**
 * Configure IPv6 address on network port interface
 *
 * The function applies to the interface associated to the port (when
 * subport_vlan is NFP_IFPORT_NET_SUBPORT_ITF) or to a VLAN.
 * VLAN interface is created if it does not exists.
 *
 * @param port Specify the network port
 * @param subport_vlan Specify the network sub-port.
 * @param vrf Virtual routing table
 * @param addr IPv6 address to set
 * @param masklen Mask length
 * @param if_sp_mgmt Slow path interface management for the newly
 * created VLAN interface
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_net_ipv6_up(int port, uint16_t subport_vlan,
				   uint8_t *addr, int masklen,
				   odp_bool_t if_sp_mgmt);

/**
 * Add an IPv4 address on network port interface
 *
 * The function applies to the interface associated to the port (when
 * subport_vlan is NFP_IFPORT_NET_SUBPORT_ITF) or to a VLAN.
 * VLAN interface must exist.
 *
 * @param port Specify the network port
 * @param subport_vlan Specify the network sub-port.
 * @param vrf Virtual routing table
 * @param addr IPv4 address to set
 * @param masklen Mask length
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_net_ipv4_addr_add(int port, uint16_t subport_vlan,
					 uint16_t vrf,
					 uint32_t addr, int masklen);

/**
 * Delete an IPv4 address from network port interface
 *
 * The function applies to the interface associated to the port (when
 * subport_vlan is NFP_IFPORT_NET_SUBPORT_ITF) or to a VLAN.
 *
 * @param port Specify the network port
 * @param subport_vlan Specify the network sub-port.
 * @param vrf Virtual routing table
 * @param addr IPv4 address to delete
 * @param masklen Mask length
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_net_ipv4_addr_del(int port, uint16_t subport_vlan,
					 int vrf, uint32_t addr, int masklen);

/**
 * Configure a IPv4 tunnel interface
 *
 * Interface is created if it does not exists.
 *
 * @param port Port associated with the tunnel type (e.g. NFP_IFPORT_GRE)
 * @param subport Sub-port associated with the interface.
 * @param vrf Virtual routing table
 * @param tun_loc Tunnel local address
 * @param tun_loc Tunnel remote address
 * @param p2p Peer address
 * @param addr IPv4 address to set
 * @param masklen Mask length
 * @param if_sp_mgmt Slow path interface management
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_tun_ipv4_up(int port, uint16_t subport,
				   uint16_t vrf, uint32_t tun_loc,
				   uint32_t tun_rem, uint32_t p2p,
				   uint32_t addr, int masklen,
				   odp_bool_t if_sp_mgmt);

/**
 * Configure an IPv4 VXLAN interface
 *
 * Interface is created if it does not exists.
 * Port is always NFP_IFPORT_VXLAN.
 *
 * @param subport_vni VXLAN Network Identifier
 * @param group Multicast IP Address group
 * @param endpoint_port Endpoint interface port
 * @param endpoint_subport Endpoint interface sub-port
 * @param addr IPv4 address to set
 * @param masklen Mask length
 * @param if_sp_mgmt Slow path interface management
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_vxlan_ipv4_up(int subport_vni, uint32_t group,
				     int endpoint_port, int endpoint_subport,
				     uint32_t addr, int masklen,
				     odp_bool_t if_sp_mgmt);

/**
 * Configure IPv4 address on a local port interface
 *
 * Local interface is created if it does not exists.
 * Port is always NFP_IFPORT_LOCAL.
 *
 * @param subport_id Local interface ID
 * @param vrf Virtual routing table
 * @param addr IPv4 address to set
 * @param masklen Mask length
 * @param if_sp_mgmt Slow path interface management
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_local_ipv4_up(uint16_t subport_id, uint16_t vrf,
				     uint32_t addr, int masklen,
				     odp_bool_t if_sp_mgmt);

/**
 * Configure IPv6 address on a local port interface
 *
 * Local interface must exist. It can be created with
 * nfp_ifport_local_ipv4_up() function.
 * Port is always NFP_IFPORT_LOCAL.
 *
 * @param subport_id Local interface ID
 * @param addr IPv6 address to set
 * @param masklen Mask length
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_local_ipv6_up(uint16_t subport_id,
				     uint8_t *addr, int masklen);

/**
 * Unconfigure an NFP interface
 *
 * Cleans up the interface resources (e.g routes and addresses).
 * If the interface coresponds to a subport (e.g. vlan), the interfaces
 * is deleted.
 *
 * @param port Specify the network port
 * @param subport Specify the network sub-port
 *
 * @retval NULL on success
 * @retval error message on error
 */
const char *nfp_ifport_ifnet_down(int port, uint16_t subport);

/**
 * Show interfaces configuration
 *
 * @param pr NFP printer to use
 */
void nfp_ifport_ifnet_show(nfp_print_t *pr);

/**
 *  Show IPv4 address configured on network ports
 *
 * @param pr NFP printer to use
 */
void nfp_ifport_net_ipv4_addr_show(nfp_print_t *pr);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_PORTCONF_H__ */

