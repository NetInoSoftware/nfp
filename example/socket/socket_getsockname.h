/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_GETSOCKNAME_H__
#define __SOCKET_GETSOCKNAME_H__

int getsockname_bind(int fd);
int getsockname_not_bind(int fd);

#ifdef INET6
int getsockname_bind6(int fd);
int getsockname_not_bind6(int fd);
#endif /* INET6 */

#endif /* __SOCKET_GETSOCKNAME_H__ */
