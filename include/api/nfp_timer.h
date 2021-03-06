/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_TIMER_H__
#define __NFP_TIMER_H__

#include <odp_api.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef void (*nfp_timer_callback)(void *arg);

odp_timer_t nfp_timer_start(uint64_t tmo_us, nfp_timer_callback callback,
		       void *arg, int arglen);
int nfp_timer_cancel(odp_timer_t tim);
void nfp_timer_handle(odp_event_t buf);
int nfp_timer_ticks(int timer_num);
odp_timer_pool_t nfp_timer(int timer_num);

/** Register a timeout callback that will be posted on the specified cpu timer
 * queue. */
odp_timer_t nfp_timer_start_cpu_id(uint64_t tmo_us, nfp_timer_callback callback,
		       void *arg, int arglen, int cpu_id);
/** Get timer queue for a cpu id. */
odp_queue_t nfp_timer_queue_cpu(int cpu_id);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_TIMER_H__ */
