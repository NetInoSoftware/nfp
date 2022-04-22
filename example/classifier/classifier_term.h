/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_CLASSIFIER_TERM_H__
#define __NFP_CLASSIFIER_TERM_H__

#include "nfp.h"

int init_posix(uint16_t port);
int cleanup_posix(int sd);
int termination_posix(void *_arg);

nfp_channel_t init_channel(void);
int cleanup_channel(nfp_channel_t chn);
void channel_packet_input(nfp_pkt_vector_t *vec, nfp_channel_t chn);
int termination_channel(void *_arg);

#endif /* __NFP_CLASSIFIER_TERM_H__ */
