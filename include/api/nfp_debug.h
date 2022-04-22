/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_DEBUG_H__
#define __NFP_DEBUG_H__

#include <odp_api.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Debug configure interface
 */
#define NFP_DEBUG_PRINT_RECV_NIC 1
#define NFP_DEBUG_PRINT_SEND_NIC 2
#define NFP_DEBUG_PRINT_RECV_KNI 4
#define NFP_DEBUG_PRINT_SEND_KNI 8
#define NFP_DEBUG_PRINT_CONSOLE 16
#define NFP_DEBUG_CAPTURE       32

void nfp_set_debug_flags(int flags);
int nfp_get_debug_flags(void);

#define NFP_DEBUG_PCAP_PORT_MASK 0x3f
#define NFP_DEBUG_PCAP_CONF_ADD_INFO 0x80000000

/*
 * Debug PCAP interface
 */
void nfp_set_debug_capture_ports(int ports);
int nfp_get_debug_capture_ports(void);

void nfp_set_capture_file(const char *filename);
void nfp_get_capture_file(char *filename, int max_size);

void nfp_set_print_file(const char *filename);
void nfp_get_print_file(char *filename, int max_size);

/*
 * Debug PRINT interface
 */
void nfp_print_packet(const char *comment, odp_packet_t pkt);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /*__NFP_DEBUG_H__*/
