/* Copyright (c) 2015, ENEA Software AB
 * Copyright (c) 2015, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_IFNET_H__
#define __NFP_IFNET_H__

#include <odp_api.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef void *nfp_ifnet_t;
#define NFP_IFNET_INVALID NULL

enum nfp_ifnet_ip_type {
	NFP_IFNET_IP_TYPE_IP_ADDR = 0,
	NFP_IFNET_IP_TYPE_P2P,
	NFP_IFNET_IP_TYPE_TUN_LOCAL,
	NFP_IFNET_IP_TYPE_TUN_REM
};

/**
 * Get interface port and subport
 *
 * @param ifnet Interface
 * @param port Interface port
 * @param subport Interface sub-port
 * @retval -1 on error
 * @retval 0 on success
 */
int nfp_ifnet_port_get(nfp_ifnet_t ifnet, int *port, uint16_t *subport);

/**
 * Get interface IPv4 address
 *
 * @param ifnet Interface
 * @param type Address type to get
 * @param addr IPv4 address
 * @retval -1 on error
 * @retval 0 on success
 */
int nfp_ifnet_ipv4_addr_get(nfp_ifnet_t ifnet, enum nfp_ifnet_ip_type type,
			    uint32_t *addr);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IFNET_H__ */
