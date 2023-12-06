/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __SOCKET_SENDMSG_RECVMSG_H__
#define __SOCKET_SENDMSG_RECVMSG_H__

int sendmsg_ip4_udp_local_ip(int fd);
int sendmsg_multi_ip4_udp_local_ip(int fd);

int connect_sendmsg_tcp_local_ip(int fd);
int connect_sendmsg_multi_tcp_local_ip(int fd);

#ifdef INET6
int sendmsg_multi_ip6_udp_local_ip(int fd);
int connect_sendmsg_multi_ipv6_tcp_local_ip(int fd);
#endif /* INET6 */

int recvmsg_udp(int fd);
int recvmsg_multi_udp(int fd);
int recvmsg_multi_peek_udp(int fd);

int accept_recvmsg_tcp(int fd);
int accept_recvmsg_multi_tcp(int fd);
int accept_recvmsg_multi2_tcp(int fd);
int accept_recvmsg_multi_peek_tcp(int fd);

int send_tcp4_2_local_ip(int fd);

#endif /* __SOCKET_SENDMSG_RECVMSG_H__ */
