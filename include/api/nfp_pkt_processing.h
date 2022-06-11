/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_PKT_PROC_H__
#define __NFP_PKT_PROC_H__

#include <odp_api.h>
#include "nfp_types.h"
#include "nfp_pkt_vector.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef enum nfp_return_code (*nfp_pkt_processing_func)(odp_packet_t *pkt);
typedef int (*nfp_pkt_processing_multi_func)(nfp_pkt_vector_t *vec);

struct nfp_ifnet;

/**
 * Default event dispatcher
 *
 * The event dispatcher contains the main processing loop for a NFP worker
 * thread or process. It retrives the ODP events (packets, timer events, etc.)
 * and calls event processing functions.
 *
 * The default event dispather is an exemple implementation using ODP scheduler
 * API. Application may construct and use a custom event dispatcher more
 * adapted to the application needs.
 *
 * @param arg   'nfp_pkt_processing_func' type function used for packet
 *              processing
 * @return      0 success, -1 error
 */

int default_event_dispatcher(void *arg);

/**
 * Default event dispatcher (multi-packet API aware)
 *
 * The event dispatcher contains the main processing loop for a NFP worker
 * thread or process. It retrives the ODP events (packets, timer events, etc.)
 * and calls event processing functions.
 *
 * The default event dispather is an exemple implementation using ODP scheduler
 * API. Application may construct and use a custom event dispatcher more
 * adapted to the application needs.
 *
 * @param arg   'nfp_pkt_processing_multi_func' type function used for packet
 *              processing
 * @return      0 success, -1 error
 */
int default_event_dispatcher_multi(void *arg);

/**
 * Return the minimum size of the user area that must be present in all
 * ODP packets passed to NFP.
 */
uint32_t nfp_packet_min_user_area(void);

/**
 * Input a packet and process it using the function supplied by the
 * caller.
 *
 * @param pkt      Packet to process. pkt_func may require some of
 *                 L2/L3/L4 offsets to be set.
 * @param in_queue ODP queue from which the packet was dequeued, or
 *                 ODP_QUEUE_INVALID.
 * @param pkt_func Packet processing function. This may be one of the
 *                 nfp_*_processing() functions.
 */
enum nfp_return_code nfp_packet_input(odp_packet_t pkt,
				      odp_queue_t in_queue,
				      nfp_pkt_processing_func pkt_func);

/**
 * Input a packet vector and process it using the function supplied
 * by the caller.
 *
 * @param vec      Packet vector to process. pkt_multi_func may require
 *                 some of L2/L3/L4 offsets to be set.
 * @param in_queue ODP queue from which the packet was dequeued, or
 *                 ODP_QUEUE_INVALID.
 * @param pkt_multi_func Packet processing function. This may be one of the
 *                 nfp_*_multi_processing() functions.
 */
int nfp_packet_input_multi(nfp_pkt_vector_t *vec,
			   odp_queue_t in_queue,
			   nfp_pkt_processing_multi_func pkt_multi_func);

/**
 * Process a packet, starting with L2.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L2 and L3 offsets must be
 *                    set.
 */
enum nfp_return_code nfp_eth_vlan_processing(odp_packet_t *pkt);

/**
 * Process multiple packets, starting with L2.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L2 and L3 offsets must be
 *                    set.
 */
int nfp_eth_vlan_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Process an IPv4 packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_ipv4_processing(odp_packet_t *pkt);

/**
 * Process multiple IPv4 packets.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L3 offset must be set.
 */
int nfp_ipv4_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Process an IPv6 packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_ipv6_processing(odp_packet_t *pkt);

/**
 * Process multiple IPv6 packets.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L3 offset must be set.
 */
int nfp_ipv6_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Process a GRE packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_gre_processing(odp_packet_t *pkt);

/**
 * Process an ARP packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_arp_processing(odp_packet_t *pkt);

/**
 * Process multiple ARP packets.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L3 offset must be set.
 */
int nfp_arp_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Process a UDP packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_udp4_processing(odp_packet_t *pkt);

/**
 * Process multiple UDP packets.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L3 offset must be set.
 */
int nfp_udp4_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Process a TCP packet.
 *
 * To be used with nfp_packet_input(), not to be called directly.
 *
 * @param[in,out] pkt Packet to process. L3 offset must be set.
 */
enum nfp_return_code nfp_tcp4_processing(odp_packet_t *pkt);

/**
 * Process multiple TCP packets.
 *
 * To be used with nfp_packet_input_multi(), not to be called directly.
 *
 * @param[in,out] vec Packets to process. L3 offset must be set.
 */
int nfp_tcp4_processing_multi(nfp_pkt_vector_t *vec);

/**
 * Force the sending of pending packets of the current thread.
 *
 * When burst sending is configured (see pkt_tx_burst_size), it forces the
 * sending of the currently stored packets (before reaching the full burst
 * size).
 */
enum nfp_return_code nfp_send_pending_pkt(void);

/**
 * Enable packet send burst on the current thread.
 *
 * For a control thread, it changes the tx burst size to pkt_tx_burst_size (from
 * the default 1).
 * For a worker thread, the tx burst size remains unchanged (pkt_tx_burst_size).
 *
 * @return      0 success, -1 error
 */
int nfp_send_burst_enable(void);

enum nfp_return_code nfp_send_frame(struct nfp_ifnet *dev, odp_packet_t pkt);

enum nfp_return_code nfp_ip_send(odp_packet_t pkt,
				 struct nfp_nh_entry *nh_param);
enum nfp_return_code nfp_ip6_send(odp_packet_t pkt,
				  struct nfp_nh6_entry *nh_param);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_PKT_PROC_H__ */
