/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_GETSERVENT_H__
#define __SOCKET_GETSERVENT_H__

int getservbyname_null_type(int fd);
int getservbyname_type(int fd);
int getservbyport_null_type(int fd);
int getservbyport_type(int fd);

#endif /* __SOCKET_GETSERVENT_H__ */
