/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_GETNAMEINFO_H__
#define __SOCKET_GETNAMEINFO_H__

extern const char * const ai_errcodes[];

int getnameinfo_ipv4_service_only(int fd);
int getnameinfo_ipv4_host_numeric_only(int fd);
int getnameinfo_ipv4_host_numeric_service(int fd);

#ifdef INET6
int getnameinfo_ipv6_service_only(int fd);
int getnameinfo_ipv6_host_numeric_only(int fd);
int getnameinfo_ipv6_host_numeric_service(int fd);
#endif /* INET6 */
#endif /* __SOCKET_GETNAMEINFO_H__ */
