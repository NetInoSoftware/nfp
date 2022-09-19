/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_DNS_COMMON_H__
#define __NFP_DNS_COMMON_H__

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define NFP_DNS_LABEL_MAX 63
#define NFP_DNS_LABEL_TXT_SIZE (NFP_DNS_LABEL_MAX + 1)
#define NFP_DNS_DOMAIN_MAX 255
#define NFP_DNS_DOMAIN_TXT_SIZE (NFP_DNS_DOMAIN_MAX + 1)

#define NFP_DNS_ROOT "."

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_DNS_COMMON_H__ */
