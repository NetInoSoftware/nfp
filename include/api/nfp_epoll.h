/* Copyright (c) 2016, Nokia
 * Copyright (c) 2016, ENEA Software AB
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_EPOLL_H__
#define __NFP_EPOLL_H__

#include <stdint.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

typedef union nfp_epoll_data {
	void    *ptr;
	int      fd;
	uint32_t u32;
	uint64_t u64;
} nfp_epoll_data_t;

struct nfp_epoll_event {
	uint32_t events;
	nfp_epoll_data_t data;
};

enum NFP_EPOLL_EVENTS {
	NFP_EPOLLIN = 0x001,
	NFP_EPOLLOUT = 0x002,
	NFP_EPOLLPRI = 0x004,
	NFP_EPOLLHUP = 0x008,
	NFP_EPOLLRDHUP = 0x010,
	NFP_EPOLLONESHOT = 0x20,
#define NFP_EPOLLIN NFP_EPOLLIN
#define NFP_EPOLLOUT NFP_EPOLLOUT
#define NFP_EPOLLPRI NFP_EPOLLPRI
#define NFP_EPOLLHUP NFP_EPOLLHUP
#define NFP_EPOLLRDHUP NFP_EPOLLRDHUP
#define NFP_EPOLLONESHOT NFP_EPOLLONESHOT
};

#define NFP_EPOLL_CTL_ADD 1
#define NFP_EPOLL_CTL_DEL 2
#define NFP_EPOLL_CTL_MOD 3

int nfp_epoll_create(int size);

int nfp_epoll_ctl(int epfd, int op, int fd, struct nfp_epoll_event *event);

int nfp_epoll_wait(int epfd, struct nfp_epoll_event *events, int maxevents, int timeout);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif
