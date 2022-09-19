/* Copyright (c) 2014, Nokia
 * Copyright (c) 2014, ENEA Software AB
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_STAT_H__
#define __NFP_STAT_H__

#include <odp_api.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define NFP_LATENCY_SLICES 64

struct nfp_packet_stat {
	struct ODP_ALIGNED_CACHE {
		uint64_t rx_fp;
		uint64_t tx_fp;
		uint64_t rx_sp;
		uint64_t tx_sp;
		uint64_t tx_eth_frag;
		uint64_t rx_ip_frag;
		uint64_t rx_ip_reass;
		uint64_t input_latency[NFP_LATENCY_SLICES];
		odp_time_t last_input_cycles;
	} per_thr[ODP_THREAD_COUNT_MAX];
};

struct nfp_perf_stat {
	uint64_t rx_fp_pps;
	uint64_t rx_prev_sum;
};

/* Stats: Get stats */
struct nfp_packet_stat *nfp_get_packet_statistics(void);
struct nfp_perf_stat *nfp_get_perf_statistics(void);

/* Stats: configure*/
#define NFP_STAT_COMPUTE_LATENCY 1
#define NFP_STAT_COMPUTE_PERF 2

void nfp_set_stat_flags(unsigned long int flags);
unsigned long int nfp_get_stat_flags(void);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_STAT_H__ */
