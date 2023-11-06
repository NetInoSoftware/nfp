/*-
 * Copyright (c) 2014 ENEA Software AB
 * Copyright (c) 2014 Nokia
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef _UDP_FWD_H_
#define _UDP_FWD_H_

#define TEST_LPORT 5001
#define TEST_RPORT 5000

int udp_fwd_cfg(int sock_count, char *laddr_txt, char *raddr_txt);
int udp_fwd_cleanup(void);

enum nfp_return_code packet_hook_ipv4(odp_packet_t pkt, void *arg);
enum nfp_return_code packet_hook_udpv4(odp_packet_t pkt, void *arg);

#endif /*_UDP_FWD_H_*/
