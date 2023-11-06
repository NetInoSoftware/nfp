/* Copyright (c) 2021 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_PRINT_H__
#define __NFP_PRINT_H__

#include <string.h>
#include "nfp_types.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef struct nfp_print_s {
	int           fd;
	int (*print_cb)(struct nfp_print_s *pr, char *buf, nfp_size_t buf_size);
} nfp_print_t;

enum nfp_print_type {
	NFP_PRINT_FILE,
	NFP_PRINT_NFP_SOCK,
	NFP_PRINT_LINUX_SOCK
};

void nfp_print_init(nfp_print_t *pr, int fd, enum nfp_print_type type);

int nfp_print_buffer(nfp_print_t *pr, char *buf, nfp_size_t buf_size);
int nfp_print(nfp_print_t *pr, const char *fmt, ...);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_PRINT_H__ */

