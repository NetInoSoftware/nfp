/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_UDP_TERM_SOCKET_H__
#define __NFP_UDP_TERM_SOCKET_H__

#define TEST_LPORT 5001

extern uint64_t received_total;
extern uint64_t received_pkt_total;

int udp_socket_cfg(char *laddr, uint16_t lport, nfp_channel_t *chn);
int udp_socket_cleanup(int sd);

nfp_channel_t udp_channel_config(void);
int udp_channel_cleanup(nfp_channel_t chn);

int termination_posix(void *arg);
int termination_channel(void *arg);

#endif /* __NFP_UDP_TERM_SOCKET_H__ */
