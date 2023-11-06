/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "errno.h"
#include "nfp.h"
#include "socket_util.h"
#include "socket_sendmsg_recvmsg.h"

#define SENDMSG1_TXT "socket"
#define SENDMSG2_TXT "_test"

#define RECVBUFF1_SIZE 4
#define RECVBUFF2_SIZE 8
#define RECVBUFF_SIZE 100

int sendmsg_ip4_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov = {0};
	char buf[20];
	int res;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);

	msg.msg_iovlen = 1;
	msg.msg_iov = &iov;

	strcpy(buf, SENDMSG1_TXT);
	iov.iov_base = buf;
	iov.iov_len = strlen(buf);

	sleep(1);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != strlen(buf)) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int sendmsg_multi_ip4_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	char buf[20];
	char buf2[20];
	int res;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;

	strcpy(buf, SENDMSG1_TXT);
	iov[0].iov_base = buf;
	iov[0].iov_len = strlen(buf);

	strcpy(buf2, SENDMSG2_TXT);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);

	sleep(1);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != strlen(buf) + strlen(buf2)) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_sendmsg_tcp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov = {0};
	char buf[20];
	int res;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			 sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 1;
	msg.msg_iov = &iov;

	strcpy(buf, SENDMSG1_TXT);
	iov.iov_base = buf;
	iov.iov_len = strlen(buf);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != strlen(buf)) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_sendmsg_multi_tcp_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	char buf[20];
	char buf2[20];
	int res;

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if (nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;

	strcpy(buf, SENDMSG1_TXT);
	iov[0].iov_base = buf;
	iov[0].iov_len = strlen(buf);

	strcpy(buf2, SENDMSG2_TXT);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != (strlen(buf) + strlen(buf2))) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#ifdef INET6
int sendmsg_multi_ip6_udp_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	char buf[20];
	char buf2[20];
	int res;

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;

	strcpy(buf, SENDMSG1_TXT);
	iov[0].iov_base = buf;
	iov[0].iov_len = strlen(buf);

	strcpy(buf2, SENDMSG2_TXT);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);

	sleep(1);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != strlen(buf) + strlen(buf2)) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int connect_sendmsg_multi_ipv6_tcp_local_ip(int fd)
{
	struct nfp_sockaddr_in6 addr = {0};
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	char buf[20];
	char buf2[20];
	int res;

	addr.sin6_len = sizeof(struct nfp_sockaddr_in6);
	addr.sin6_family = NFP_AF_INET6;
	addr.sin6_port = odp_cpu_to_be_16(TEST_PORT + 1);
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr.sin6_addr.nfp_s6_addr);

	if (nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			sizeof(addr)) == -1) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;

	strcpy(buf, SENDMSG1_TXT);
	iov[0].iov_base = buf;
	iov[0].iov_len = strlen(buf);

	strcpy(buf2, SENDMSG2_TXT);
	iov[1].iov_base = buf2;
	iov[1].iov_len = strlen(buf2);

	res = nfp_sendmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to send data(errno = %d)", nfp_errno);
		return -1;
	}

	if ((size_t)res != (strlen(buf) + strlen(buf2))) {
		NFP_ERR("Failed to send data all data (res = %d)", res);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

#endif /* INET6 */

int recvmsg_udp(int fd)
{
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov = {0};
	uint8_t buf[RECVBUFF_SIZE];
	int res;
	union {
		struct nfp_sockaddr addr;
		struct nfp_sockaddr_in addr4;
		struct nfp_sockaddr_in6 addr6;
	} addr = {0};

	msg.msg_name = &addr;
	msg.msg_namelen = sizeof(addr);

	msg.msg_iovlen = 1;
	msg.msg_iov = &iov;
	iov.iov_base = buf;
	iov.iov_len = RECVBUFF_SIZE;

	res = nfp_recvmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		return -1;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, res);

	if (msg.msg_namelen <= 0) {
		NFP_ERR("Invalid address size (%d)", msg.msg_namelen);
		return -1;
	}

	if (addr.addr.sa_family == NFP_AF_INET)
		NFP_INFO("Data was received on address 0x%x, port = %d.\n",
			 odp_be_to_cpu_32(addr.addr4.sin_addr.s_addr),
			 odp_be_to_cpu_16(addr.addr4.sin_port));
	else if (addr.addr.sa_family == NFP_AF_INET6) {
		struct nfp_sockaddr_in6 *addr6 = &addr.addr6;

		NFP_INFO("Data was received on address %x:%x:%x:%x, "
			 "port = %d.\n",
			 odp_be_to_cpu_32(addr6->sin6_addr.nfp_s6_addr32[0]),
			 odp_be_to_cpu_32(addr6->sin6_addr.nfp_s6_addr32[1]),
			 odp_be_to_cpu_32(addr6->sin6_addr.nfp_s6_addr32[2]),
			 odp_be_to_cpu_32(addr6->sin6_addr.nfp_s6_addr32[3]),
			 odp_be_to_cpu_16(addr6->sin6_port));
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int recvmsg_multi_udp(int fd)
{
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	uint8_t buf1[RECVBUFF1_SIZE];
	uint8_t buf2[RECVBUFF2_SIZE];
	uint8_t buf[RECVBUFF_SIZE];
	uint8_t *data;
	int res, to_copy, remain, i;

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;
	iov[0].iov_base = buf1;
	iov[0].iov_len = RECVBUFF1_SIZE;
	iov[1].iov_base = buf2;
	iov[1].iov_len = RECVBUFF2_SIZE;

	res = nfp_recvmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		return -1;
	}

	for (i = 0, remain = res, data = buf; i < msg.msg_iovlen && remain > 0;
	     i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int recvmsg_multi_peek_udp(int fd)
{
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	uint8_t buf1[RECVBUFF1_SIZE];
	uint8_t buf2[RECVBUFF2_SIZE];
	uint8_t buf[RECVBUFF_SIZE];
	uint8_t *data;
	int res, to_copy, remain, i;

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;
	iov[0].iov_base = buf1;
	iov[0].iov_len = RECVBUFF1_SIZE;
	iov[1].iov_base = buf2;
	iov[1].iov_len = RECVBUFF2_SIZE;

	/* peek */
	res = nfp_recvmsg(fd, &msg, NFP_MSG_PEEK);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		return -1;
	}

	for (i = 0, remain = res, data = buf; i < msg.msg_iovlen && remain > 0;
	     i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	/* take data*/
	res = nfp_recvmsg(fd, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		return -1;
	}

	for (i = 0, remain = res, data = buf; i < msg.msg_iovlen && remain > 0;
	     i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_recvmsg_tcp(int fd)
{
	int fd_accepted = -1;
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov = {0};
	uint8_t buf[RECVBUFF_SIZE];
	int res;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 1;
	msg.msg_iov = &iov;
	iov.iov_base = buf;
	iov.iov_len = RECVBUFF_SIZE - 1;

	sleep(1);

	res = nfp_recvmsg(fd_accepted, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;

	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_recvmsg_multi_tcp(int fd)
{
	int fd_accepted = -1;
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	uint8_t buf1[RECVBUFF1_SIZE];
	uint8_t buf2[RECVBUFF2_SIZE];
	uint8_t buf[RECVBUFF_SIZE];
	uint8_t *data;
	int res, to_copy, remain, i;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;
	iov[0].iov_base = buf1;
	iov[0].iov_len = RECVBUFF1_SIZE;
	iov[1].iov_base = buf2;
	iov[1].iov_len = RECVBUFF2_SIZE;

	/* peek */
	res = nfp_recvmsg(fd_accepted, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	for (i = 0, remain = res, data = buf; i < msg.msg_iovlen && remain > 0;
	     i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	buf[RECVBUFF_SIZE - 1] = 0;
	if (res < RECVBUFF_SIZE)
		buf[res] = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_recvmsg_multi2_tcp(int fd)
{
	int fd_accepted = -1;
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	uint8_t buf1[RECVBUFF1_SIZE];
	uint8_t buf2[RECVBUFF2_SIZE];
	uint8_t buf[RECVBUFF_SIZE];
	uint8_t *data;
	int res, to_copy, remain, i;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;
	iov[0].iov_base = buf1;
	iov[0].iov_len = RECVBUFF1_SIZE;
	iov[1].iov_base = buf2;
	iov[1].iov_len = RECVBUFF2_SIZE;

	res = nfp_recvmsg(fd_accepted, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	data = buf;
	for (i = 0, remain = res; i < msg.msg_iovlen && remain > 0; i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	*data = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	res = nfp_recvmsg(fd_accepted, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	data = buf;
	for (i = 0, remain = res; i < msg.msg_iovlen && remain > 0; i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	*data = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int accept_recvmsg_multi_peek_tcp(int fd)
{
	int fd_accepted = -1;
	struct nfp_msghdr msg = {0};
	struct nfp_iovec iov[2];
	uint8_t buf1[RECVBUFF1_SIZE];
	uint8_t buf2[RECVBUFF2_SIZE];
	uint8_t buf[RECVBUFF_SIZE];
	uint8_t *data;
	int res, to_copy, remain, i;

	fd_accepted = nfp_accept(fd, NULL, NULL);
	if (fd_accepted == -1) {
		NFP_ERR("FAILED to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	sleep(1);

	msg.msg_iovlen = 2;
	msg.msg_iov = iov;
	iov[0].iov_base = buf1;
	iov[0].iov_len = RECVBUFF1_SIZE;
	iov[1].iov_base = buf2;
	iov[1].iov_len = RECVBUFF2_SIZE;

	/* peek */
	res = nfp_recvmsg(fd_accepted, &msg, NFP_MSG_PEEK);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	data = buf;
	for (i = 0, remain = res; i < msg.msg_iovlen && remain > 0; i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	*data = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, res, msg.msg_flags);

	res = nfp_recvmsg(fd_accepted, &msg, 0);
	if (res < 0) {
		NFP_ERR("Failed to receive data(errno = %d)", nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}

	if (res == 0) {
		NFP_ERR("Invalid receive data size (%d)", res);
		nfp_close(fd_accepted);
		return -1;
	}

	data = buf;
	for (i = 0, remain = res; i < msg.msg_iovlen && remain > 0; i++) {
		to_copy = iov[i].iov_len;
		if (remain < (int)iov[i].iov_len)
			to_copy = remain;

		memcpy(data, iov[i].iov_base, to_copy);
		remain -= to_copy;
		data += to_copy;
	}

	*data = 0;
	NFP_INFO("Data (%s, len = %d) was received (flag = 0x%x).",
		 buf, (int)(data - buf), msg.msg_flags);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("FAILED to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int send_tcp4_2_local_ip(int fd)
{
	struct nfp_sockaddr_in addr = {0};
	const char *buf1 = "socket";
	const char *buf2 = "_test";

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 1);
	addr.sin_addr.s_addr = TEST_ADDR4;

	if ((nfp_connect(fd, (struct nfp_sockaddr *)&addr,
			 sizeof(addr)) == -1) &&
		(nfp_errno != NFP_EINPROGRESS)) {
		NFP_ERR("Failed to connect (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_send(fd, buf1, strlen(buf1), 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}

	if (nfp_send(fd, buf2, strlen(buf2), 0) == -1) {
		NFP_ERR("Failed to send (errno = %d)\n", nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
