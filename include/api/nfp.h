/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_H__
#define __NFP_H__

#include <odp_api.h>
#include <odp/helper/odph_api.h>

/**
 * @file
 *
 * @brief The NetinoFastPath API
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "nfp_config.h"
#include "nfp_types.h"
#include "nfp_init.h"
#include "nfp_ifnet.h"
#include "nfp_pkt_processing.h"
#include "nfp_api_cli.h"
#include "nfp_thread_proc.h"
#include "nfp_log.h"
#include "nfp_timer.h"
#include "nfp_hook.h"
#include "nfp_route_arp.h"
#include "nfp_ifnet.h"
#include "nfp_ifnet_portconf.h"
#include "nfp_ifaddrs.h"
#include "nfp_debug.h"
#include "nfp_stat.h"
#include "nfp_socket_types.h"
#include "nfp_socket_sigevent.h"
#include "nfp_socket.h"
#include "nfp_in.h"
#include "nfp_in6.h"
#include "nfp_errno.h"
#include "nfp_ioctl.h"
#include "nfp_utils.h"
#include "nfp_sysctl.h"
#include "nfp_ethernet.h"
#include "nfp_ip.h"
#include "nfp_ip6.h"
#include "nfp_icmp.h"
#include "nfp_icmp6.h"
#include "nfp_if_vlan.h"
#include "nfp_udp.h"
#include "nfp_ip_var.h"
#include "nfp_tcp.h"
#include "nfp_epoll.h"
#include "nfp_ipsec.h"
#include "nfp_ipsec_init.h"
#include "nfp_print.h"
#include "nfp_time.h"
#include "nfp_select.h"
#include "nfp_pkt_channel.h"
#include "nfp_pkt_vector.h"
#include "nfp_netdb.h"
#include "nfp_dns.h"
#include "nfp_dns_common.h"

#ifdef __cplusplus
}
#endif

#endif /* __NFP_H__ */
