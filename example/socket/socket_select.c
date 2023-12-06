/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_select.h"
#include "socket_util.h"

int select_recv_udp(int fd)
{

	char buf[20];
	nfp_size_t buff_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_timeval timeout;
	int ret_select = 0;
	nfp_fd_set read_fd;

	NFP_FD_ZERO(&read_fd);
	NFP_FD_SET(fd, &read_fd);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	ret_select = nfp_select(fd + 1, &read_fd, NULL, NULL, &timeout);
	if (ret_select == -1) {
		NFP_ERR("Failed to select (errno = %d)\n", nfp_errno);
		return -1;
	}
	if (ret_select != 1) {
		NFP_ERR("Failed to select: invalid value returned %d\n",
			ret_select);
		return -1;
	}

	if (!NFP_FD_ISSET(fd, &read_fd)) {
		NFP_ERR("Failed: socket is not selected\n");
		return -1;
	}

	NFP_INFO("nfp_select() returned %d; socket is selected.\n",
		ret_select);

	len = nfp_recv(fd, buf, buff_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int select_recv_tcp(int fd)
{
	char buf[20];
	nfp_ssize_t len = sizeof(buf);
	int fd_accepted = -1;
	struct nfp_timeval timeout;
	int ret_select = 0;
	nfp_fd_set read_fd;

	NFP_FD_ZERO(&read_fd);
	NFP_FD_SET(fd, &read_fd);

	timeout.tv_sec = 0;
	timeout.tv_usec = 200000;

	ret_select = nfp_select(fd + 1, &read_fd, NULL, NULL, &timeout);
	if (ret_select == -1) {
		NFP_ERR("Failed to select (errno = %d)\n", nfp_errno);
		return -1;
	}
	if (ret_select != 1) {
		NFP_ERR("Failed to select: invalid value returned %d\n",
			ret_select);
		return -1;
	}

	if (!NFP_FD_ISSET(fd, &read_fd)) {
		NFP_ERR("Failed: socket is not selected\n");
		return -1;
	}
	NFP_INFO("nfp_select() returned %d; socket is selected.\n",
		ret_select);

	fd_accepted = nfp_accept(fd, NULL, NULL);

	if (fd_accepted == -1) {
		NFP_ERR("Failed to accept connection (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	len = nfp_recv(fd_accepted, buf, sizeof(buf), 0);
	if (len == -1) {
		NFP_ERR("Failed to recv (errno = %d)\n",
			nfp_errno);
		nfp_close(fd_accepted);
		return -1;
	}
	buf[len] = 0;
	NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

	if (nfp_close(fd_accepted) == -1) {
		NFP_ERR("Failed to close accepted socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}

int select_recv_udp_2(int fd)
{
	char buf[20];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_timeval timeout;
	int ret_select = 0;
	nfp_fd_set read_fd;
	struct nfp_sockaddr_in addr;
	int fd2 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				NFP_IPPROTO_UDP);
	int fd3 = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM,
				NFP_IPPROTO_UDP);

	addr.sin_len = sizeof(struct nfp_sockaddr_in);
	addr.sin_family = NFP_AF_INET;
	addr.sin_port = odp_cpu_to_be_16(TEST_PORT + 2);
	addr.sin_addr.s_addr = NFP_INADDR_ANY;

	if (nfp_bind(fd3, (const struct nfp_sockaddr *)&addr,
		sizeof(struct nfp_sockaddr_in)) == -1) {
		NFP_ERR("Failed to bind socket (errno = %d)\n",
			nfp_errno);
		nfp_close(fd2);
		nfp_close(fd3);
		return -1;
	}

	strcpy(buf, "socket_2");
	if (nfp_sendto(fd2, buf, strlen(buf), 0,
		(struct nfp_sockaddr *)&addr,
		sizeof(addr)) == -1) {
		NFP_ERR("Failed to send data(errno = %d)\n", nfp_errno);
		nfp_close(fd2);
		nfp_close(fd3);
		return -1;
	}

	NFP_FD_ZERO(&read_fd);
	NFP_FD_SET(fd, &read_fd);
	NFP_FD_SET(fd3, &read_fd);

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	ret_select = nfp_select(fd3 + 1, &read_fd, NULL, NULL, &timeout);
	if (ret_select == -1) {
		NFP_ERR("Failed to select (errno = %d)\n", nfp_errno);
		nfp_close(fd2);
		nfp_close(fd3);
		return -1;
	}

	if (!NFP_FD_ISSET(fd, &read_fd)) {
		NFP_ERR("Failed: socket is not selected\n");
		return -1;
	}
	if (!NFP_FD_ISSET(fd3, &read_fd)) {
		NFP_ERR("Failed: socket is not selected\n");
		return -1;
	}
	NFP_INFO("nfp_select() returned %d; sockets are selected.\n",
		ret_select);

	len = nfp_recv(fd, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		nfp_close(fd2);
		nfp_close(fd3);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data1 (%s, len = %d) was received.\n", buf, len);

	len = nfp_recv(fd3, buf, buf_len, 0);
	if (len == -1) {
		NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
		nfp_close(fd2);
		nfp_close(fd3);
		return -1;
	}

	buf[len] = 0;
	NFP_INFO("Data2 (%s, len = %d) was received.\n", buf, len);

	nfp_close(fd2);
	nfp_close(fd3);

	NFP_INFO("SUCCESS.\n");
	return 0;
}

