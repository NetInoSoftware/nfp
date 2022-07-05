/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_GETADDRINFO_H__
#define __SOCKET_GETADDRINFO_H__

extern const char * const ai_errcodes[];

int gai_strerror_print(int fd);

int getaddrinfo_null_hints_null_host(int fd);
int getaddrinfo_null_hints_num_host(int fd);
int getaddrinfo_null_hints_num6_host(int fd);

int getaddrinfo_ipv4_hints_null_host(int fd);
int getaddrinfo_ipv6_hints_null_host(int fd);

int getaddrinfo_ai_passive_hints_null_host(int fd);
int getaddrinfo_ai_canonname_hints_num_host(int fd);

int getaddrinfo_ipv6_ai_v4mapped_hints_num_host(int fd);
int getaddrinfo_ai_numerichost_hints_num_host(int fd);
int getaddrinfo_ai_addrconfig_hints_num_host(int fd);
int getaddrinfo_localhost(int fd);
int getaddrinfo_localhost_service(int fd);

#endif /* __SOCKET_GETADDRINFO_H__ */
