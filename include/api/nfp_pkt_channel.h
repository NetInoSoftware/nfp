/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_PKT_CHANNEL_H__
#define __NFP_PKT_CHANNEL_H__

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#include "odp_api.h"
#include "nfp_types.h"

typedef uint32_t nfp_channel_t;

#define NFP_CHANNEL_INVALID ((uint32_t)(-1))

typedef enum {
	/* Channel takes ownership of the packets */
	NFP_CHANNEL_METHOD_TAKE = 0,
	/* Channel creates a copy of the packets */
	NFP_CHANNEL_METHOD_COPY,
	/* Channel creates a dynamic reference of the packets */
	NFP_CHANNEL_METHOD_REF
} nfp_channel_method_t;

typedef enum {
	/* User data is stored into the packet user area.
	Only up to the maximum size of the packet user area is copied.*/
	NFP_CHANNEL_UA_LOCATION_PKT_UA = 0,
	/* User data is stored into the packet body.*/
	NFP_CHANNEL_UA_LOCATION_PKT_DATA
} nfp_channel_ua_location_t;

typedef struct nfp_channel_param_t {
	/* Packet trasffer method.*/
	nfp_channel_method_t method;

	/* Storage location for the user data */
	nfp_channel_ua_location_t ua_location;

	/* ODP queue parameters */
	odp_queue_param_t queue_param;
} nfp_channel_param_t;

/**
 * Initialize channel parameters
 *
 * Initialize an nfp_channel_param_t to its default values for all fields.
 *
 * @param param   Address of the nfp_channel_param_t to be initialized
 */
void nfp_channel_param_init(nfp_channel_param_t *param);

/**
 * Channel create
 *
 * Create a channel according to the parameters
 *
 * @return Channel handle
 * @return NFP_CHANNEL_INVALID on failure
 */
nfp_channel_t nfp_channel_create(const char *name,
				 const nfp_channel_param_t *param);

/**
 * Destroy NFP channel
 *
 * Destroy NFP channel.
 *
 * @param channel    Channel handle
 *
 * @retval 0 on success
 * @retval <0 on failure
 */
int nfp_channel_destroy(nfp_channel_t channel);

/**
 * Send packets to the channel
 *
 * @param channel     Channel handle
 * @param pkt         Array of 'num' packets to be sent.
 * @param ua          Memory area containing user data to be stored with the
 *                    packets.
 *                    If not NULL, must contain 'num' * 'ua_pkt_size' bytes.
 *                    Each packet will store 'ua_pkt_size' bytes.
 * @param ua_pkt_size The size of user data to be store with a packet. Is
 *                    ignored if ua is NULL.
 * @param num         Number of packets to send
 *
 * @retval Number of packets successfully sent
 * @retval <0 on failure
 */
int nfp_channel_send(nfp_channel_t channel, odp_packet_t *pkt,
		     uint8_t *ua, nfp_size_t ua_pkt_size, int num);

/**
 * Receive packets from the channel
 *
 * @param channel     Channel handle
 * @param[out] pkt    Array of received packets. Must have 'num_max' size.
 * @param[out] ua     Memory area containing user data received with the
 *                    packets.
 *                    If not NULL, it must have 'num_max' * 'ua_pkt_size' bytes.
 * @param ua_pkt_size The maximum size of user data to be received with a
 *                    packet. Is ignored if ua is NULL.
 * @param num         Maximum number of packets to receive.
 *
 * @retval Number of packets received
 * @retval <0 on failure
 */
int nfp_channel_recv(nfp_channel_t channel, odp_packet_t *pkt,
		     uint8_t *ua, nfp_size_t ua_pkt_size, int num_max);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_PKT_CHANNEL_H__ */

