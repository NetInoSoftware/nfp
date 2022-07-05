/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_DNS_H__
#define __NFP_DNS_H__

#include <stdint.h>
#include "nfp_print.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

int nfp_dns_show(nfp_print_t *pr);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_DNS_H__ */
