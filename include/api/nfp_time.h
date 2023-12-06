/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_TIME_H__
#define __NFP_TIME_H__

#include <stdint.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif
struct nfp_timeval {
	uint32_t tv_sec;     /* seconds */
	uint32_t tv_usec;    /* microseconds */
};

struct nfp_timespec {
	uint32_t tv_sec;     /* seconds */
	uint32_t tv_nsec;    /* nanoseconds */
};

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_TIME_H__ */
