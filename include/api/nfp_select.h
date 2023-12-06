/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_SELECT_H__
#define __NFP_SELECT_H__

#include <stdint.h>
#include "nfp_config.h"
#include "nfp_time.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef struct {
	uint8_t fd_set_buf[(NFP_SELECT_SET_SIZE + 7) / 8];
} nfp_fd_set;

void NFP_FD_CLR(int fd, nfp_fd_set *set);
int  NFP_FD_ISSET(int fd, nfp_fd_set *set);
void NFP_FD_SET(int fd, nfp_fd_set *set);
void NFP_FD_ZERO(nfp_fd_set *set);

int nfp_select(int nfds, nfp_fd_set *readfds, nfp_fd_set *writefds,
	       nfp_fd_set *exceptfds, struct nfp_timeval *timeout);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_SELECT_H__ */
