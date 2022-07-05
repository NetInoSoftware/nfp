/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_GETPEERNAME_H__
#define __SOCKET_GETPEERNAME_H__

int getpeername_not_connect(int fd);
int getpeername_connect(int fd);
int getpeername_accept(int fd);

#ifdef INET6
int getpeername_not_connect6(int fd);
int getpeername_connect6(int fd);
int getpeername_accept6(int fd);
#endif /* INET6 */

#endif /* __SOCKET_GETPEERNAME_H__ */
