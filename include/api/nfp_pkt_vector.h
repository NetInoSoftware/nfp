/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_PKT_VECTOR_H__
#define __NFP_PKT_VECTOR_H__

#include <odp_api.h>
#include "nfp_types.h"
#include "nfp_config.h"

/**
 * Packet processing vector
 *
 * Packet processing vector is a mechanism designed to provide an answer
 * to the following questions:
 * - how to convey multiple packets through the chain of packet processing
 * functions
 * - how to convey the results
 * - how to treat the unprocessed packets
 *
 * It was design to integrate well with packet dispatcher functions and
 * with nfp_packet_input_multi()/SP mechanism.
 */

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef struct nfp_pkt_vector_t {
	odp_packet_t         pkt[NFP_PKT_VECTOR_SIZE];
	uint16_t             idx[NFP_PKT_VECTOR_SIZE];
	uint16_t             flg[NFP_PKT_VECTOR_SIZE];
	enum nfp_return_code ret[NFP_PKT_VECTOR_SIZE];
	uint16_t             num;
	uint16_t             num_processed;
} nfp_pkt_vector_t;

#define NFP_PKT_VECTOR_IS_FULL(_p) \
	((_p) != NULL && (_p)->num == NFP_PKT_VECTOR_SIZE)

#define NFP_PKT_VECTOR_HAS_DATA(_p) \
	((_p) != NULL && (_p)->num != 0)

#define NFP_PKT_VECTOR_HAS_UNPROCESSED(_p) \
	((_p) != NULL && (_p)->num != (_p)->num_processed)

/**
 * Initialize the packet vector
 *
 * Initialize the packet vector to its default values.
 *
 * @param vec   Packet vector
 */
void nfp_pkt_vector_init(nfp_pkt_vector_t *vec);

/**
 * Add a packet to a packet vector
 *
 * Add a packet to a packet vector on the first free position.
 *
 * @param vec   Packet vector
 * @param pkt   Packet to add
 *
 * @return Number of packet added (1 on success, 0 on error)
 */
uint16_t nfp_pkt_vector_add(nfp_pkt_vector_t *vec, odp_packet_t pkt);

/**
 * Add multiple packets to a packet vector
 *
 * Add multiple packets to a packet vector. Packets are added
 * in consecutive positions starting with the first free position.
 *
 * @param vec   Packet vector
 * @param pkt   Array of packets to add
 * @param num   Number of packets to add
 *
 * @return Number of packet added (between 0 and num)
 */
uint16_t nfp_pkt_vector_add_multi(nfp_pkt_vector_t *vec,
				  odp_packet_t *pkt, uint16_t num);

/**
 * Get packet processing result
 *
 * Get the return code resulted from processing of the packet
 * with specified index
 *
 * @param vec       Packet vector
 * @param vec_idx   Index of the packet
 * @param ret       Result value
 *
 * @return  0   Success
 * @return  1   Result not available - packet was not processed
 * @return -1   Error
 */
int nfp_pkt_vector_get_return_code(nfp_pkt_vector_t *vec, uint16_t vec_idx,
				   enum nfp_return_code *ret);

/**
 * Mark as 'dropped' the unprocessed packets
 *
 * Set the packet processing result of the unprocessed packets
 * as NFP_PKT_DROP. As consequence, the packets will be freed in
 * nfp_packet_input_multi() function.
 *
 * @param vec      Packet vector
 *
 * @return  0      Success
 * @return -1      Error
 */
int nfp_pkt_vector_drop_unprocessed(nfp_pkt_vector_t *vec);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_PKT_VECTOR_H__ */
