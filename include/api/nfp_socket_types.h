/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_SOCKET_TYPES_H__
#define __NFP_SOCKET_TYPES_H__

#include <stdint.h>
#include "nfp_types.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef uint8_t	__nfp_sa_family_t;
typedef uint32_t	__nfp_socklen_t;
typedef int32_t	__nfp_off_t;

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_SOCKET_TYPES_H__ */

