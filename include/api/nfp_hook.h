/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_HOOK_H__
#define __NFP_HOOK_H__

#include <odp_api.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/**
 * @file
 *
 * @brief Register callback functions for hook handles supported by NFP
 *
 */


/**
 * @brief Function callback format
 *
 * @param pkt The packet received by the hook callback
 * @param arg Argument structure now specifies protocol type of packet received
 * with an integer that is an nfp_hook_local_par value
 *
 * @retval nfp_return_code Control how NFP will behave after hook processing
 */
typedef enum nfp_return_code (*nfp_pkt_hook)(odp_packet_t pkt, void *arg);

/**
 * @brief Hook handles
 *
 * A function callback is called by NFP when a specific processing phase(handle)
 * is found. One can register any nfp_pkt_hook() function callback for any
 * handle.
 * The registration is done with nfp_initialize() by assigning function
 * callbacks on #nfp_initialize_param_t.pkt_hook[#nfp_hook_id]
 */
enum nfp_hook_id {
	NFP_HOOK_LOCAL = 0,	/**< Registers a function to handle all packets
					with processing at IP level */
	NFP_HOOK_LOCAL_IPv4,	/**< Registers a function to handle all packets
					with processing at IPv4 level */
	NFP_HOOK_LOCAL_IPv6,	/**< Registers a function to handle all packets
					with processing at IPv6 level */
	NFP_HOOK_LOCAL_UDPv4,	/**< Registers a function to handle all packets
					with processing at UDP IPv4 level */
	NFP_HOOK_LOCAL_UDPv6,	/**< Registers a function to handle all packets
					with processing at UDP IPv6 level */
	NFP_HOOK_FWD_IPv4,	/**< Registers a function to handle all IPv4
					packets	that require forwarding */
	NFP_HOOK_FWD_IPv6,	/**< Registers a function to handle all IPv6
					packets	that require forwarding */
	NFP_HOOK_GRE,		/**< Registers a function to handle GRE tunnels
					not registered to NFP */
	NFP_HOOK_OUT_IPv4,	/**< Registers a function to handle all IPv4
					packets to be sent by NFP*/
	NFP_HOOK_OUT_IPv6,	/**< Registers a function to handle all IPv6
					packets to be sent by NFP*/
	NFP_HOOK_MAX
};

/**
 * @brief Parameter value received as argument in hook callback
 */
enum nfp_hook_local_par {
	IS_IPV4 = 0,	/**< IPv4 packet received in hook*/
	IS_IPV6,	/**< IPv6 packet received in hook*/
	IS_IPV4_UDP,	/**< UDP over IPv4 packet received in hook*/
	IS_IPV6_UDP	/**< UDP over IPv6 packet received in hook*/
};

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_HOOK_H__ */
