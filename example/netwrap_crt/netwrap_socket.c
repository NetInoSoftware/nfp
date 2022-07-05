/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "nfp.h"
#include "netwrap_socket.h"
#include "netwrap_errno.h"

union _nfp_sockaddr_storage {
	struct nfp_sockaddr_in addr_in;
	struct nfp_sockaddr_in6 addr_in6;
};

static int (*libc_socket)(int, int, int);
static int (*libc_shutdown)(int, int);
static int (*libc_close)(int);
static int (*libc_bind)(int, const struct sockaddr*, socklen_t);
static int (*libc_accept)(int, struct sockaddr*, socklen_t*);
static int (*libc_accept4)(int, struct sockaddr*, socklen_t*, int);
static int (*libc_listen)(int, int);
static int (*libc_connect)(int, const struct sockaddr*, socklen_t);
static ssize_t (*libc_read)(int, void*, size_t);
static ssize_t (*libc_write)(int, const void*, size_t);
static ssize_t (*libc_recv)(int, void*, size_t, int);
static ssize_t (*libc_send)(int, const void*, size_t, int);

void setup_socket_wrappers(void)
{
	LIBC_FUNCTION(socket);
	LIBC_FUNCTION(shutdown);
	LIBC_FUNCTION(close);
	LIBC_FUNCTION(bind);
	LIBC_FUNCTION(accept);
	LIBC_FUNCTION(accept4);
	LIBC_FUNCTION(listen);
	LIBC_FUNCTION(connect);
	LIBC_FUNCTION(read);
	LIBC_FUNCTION(write);
	LIBC_FUNCTION(recv);
	LIBC_FUNCTION(send);
}

int socket(int domain, int type, int protocol)
{
	int sockfd = -1;

	if (netwrap_constructor_called) {
		if (domain != AF_INET)
			sockfd = (*libc_socket)(domain, type, protocol);
		else {
			int nfp_domain = NFP_AF_INET;
			int nfp_type, nfp_protocol;

			switch (type) {
			case SOCK_STREAM:
				nfp_type = NFP_SOCK_STREAM;
				break;
			case SOCK_DGRAM:
				nfp_type = NFP_SOCK_DGRAM;
				break;
			default:
				nfp_type = type;
			}

			switch (protocol) {
			case IPPROTO_UDP:
				nfp_protocol = NFP_IPPROTO_UDP;
				break;
			case IPPROTO_TCP:
				nfp_protocol = NFP_IPPROTO_TCP;
				break;
			default:
				nfp_protocol = protocol;
			}

			sockfd = nfp_socket(nfp_domain, nfp_type, nfp_protocol);
			errno = NETWRAP_ERRNO(nfp_errno);
		}
	} else { /* pre init*/
		LIBC_FUNCTION(socket);

		if (libc_socket)
			sockfd = (*libc_socket)(domain, type, protocol);
		else {
			sockfd = -1;
			errno = EACCES;
		}
	}

	/*printf("socket wrapper return: %d\n", sockfd);*/
	return sockfd;
}

int shutdown(int sockfd, int how)
{
	int shutdown_value;

	if (IS_NFP_SOCKET(sockfd)) {
		int nfp_how;

		switch (how) {
		case SHUT_RD:
			nfp_how = NFP_SHUT_RD;
			break;
		case SHUT_WR:
			nfp_how = NFP_SHUT_WR;
			break;
		case SHUT_RDWR:
			nfp_how = NFP_SHUT_RDWR;
			break;
		default:
			nfp_how = how;
		}
		shutdown_value = nfp_shutdown(sockfd, nfp_how);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_shutdown) {
		shutdown_value = (*libc_shutdown)(sockfd, how);
	} else {
		LIBC_FUNCTION(shutdown);

		if (libc_shutdown)
			shutdown_value = (*libc_shutdown)(sockfd, how);
		else {
			shutdown_value = -1;
			errno = EACCES;
		}
	}

	return shutdown_value;
}
int close(int sockfd)
{
	int close_value;

	if (IS_NFP_SOCKET(sockfd)) {
		close_value = nfp_close(sockfd);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_close)
		close_value = (*libc_close)(sockfd);
	else { /* pre init*/
		LIBC_FUNCTION(close);

		if (libc_close)
			close_value = (*libc_close)(sockfd);
		else {
			close_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Socket '%d' closed returns:'%d'\n",
		sockfd, close_value);*/
	return close_value;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int bind_value = -1;

	if (IS_NFP_SOCKET(sockfd)) {
		struct nfp_sockaddr_in nfp_addr;
		nfp_socklen_t nfp_addrlen;

		if (!addr) {
			errno = EFAULT;
			return -1;
		}
		if (addrlen != sizeof(struct sockaddr_in)) {
			errno = EINVAL;
			return -1;
		}

		bzero((char *) &nfp_addr, sizeof(nfp_addr));
		nfp_addr.sin_family = NFP_AF_INET;
		nfp_addr.sin_addr.s_addr =
			((const struct sockaddr_in *)addr)->sin_addr.s_addr;
		nfp_addr.sin_port =
			((const struct sockaddr_in *)addr)->sin_port;
		nfp_addr.sin_len = sizeof(struct nfp_sockaddr_in);

		nfp_addrlen = sizeof(nfp_addr);

		bind_value = nfp_bind(sockfd,
				(const struct nfp_sockaddr *)&nfp_addr,
				nfp_addrlen);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_bind)
		bind_value = (*libc_bind)(sockfd, addr, addrlen);
	else { /* pre init*/
		LIBC_FUNCTION(bind);

		if (libc_bind)
			bind_value = (*libc_bind)(sockfd, addr, addrlen);
		else {
			bind_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Binding socket '%d' to the address '%x:%d' returns:%d\n",
		sockfd,	((const struct sockaddr_in *)addr)->sin_addr.s_addr,
		odp_be_to_cpu_16(((const struct sockaddr_in *)addr)->sin_port),
		bind_value);*/
	return bind_value;
}


int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int accept_value = -1;

	if (IS_NFP_SOCKET(sockfd)) {
		union _nfp_sockaddr_storage nfp_addr_local;
		struct nfp_sockaddr *nfp_addr;
		nfp_socklen_t nfp_addrlen_local;
		nfp_socklen_t *nfp_addrlen;

		if (addr) {
			nfp_addr = (struct nfp_sockaddr *)&nfp_addr_local;

			if (!addrlen) {
				errno = EINVAL;
				return -1;
			}
			nfp_addrlen = &nfp_addrlen_local;
			nfp_addrlen_local = sizeof(nfp_addr_local);
		} else {
			nfp_addr = NULL;
			nfp_addrlen = NULL;
		}


		accept_value = nfp_accept(sockfd, nfp_addr, nfp_addrlen);
		errno = NETWRAP_ERRNO(nfp_errno);

		if (accept_value != -1 && addr) {
			switch (nfp_addr->sa_family) {
			case NFP_AF_INET:
			{
				struct sockaddr_in addr_in_tmp;
				struct nfp_sockaddr_in *nfp_addr_in_tmp =
					(struct nfp_sockaddr_in *)nfp_addr;

				addr_in_tmp.sin_family = AF_INET;
				addr_in_tmp.sin_port =
					nfp_addr_in_tmp->sin_port;
				addr_in_tmp.sin_addr.s_addr =
					nfp_addr_in_tmp->sin_addr.s_addr;

				if (*addrlen > sizeof(addr_in_tmp))
					*addrlen = sizeof(addr_in_tmp);

				memcpy(addr, &addr_in_tmp, *addrlen);
				break;
			}
			case NFP_AF_INET6:
			{
				struct sockaddr_in6 addr_in6_tmp;
				struct nfp_sockaddr_in6 *nfp_addr_in6_tmp =
					(struct nfp_sockaddr_in6 *)nfp_addr;

				addr_in6_tmp.sin6_family = AF_INET6;
				addr_in6_tmp.sin6_port =
					nfp_addr_in6_tmp->sin6_port;

				addr_in6_tmp.sin6_flowinfo =
					nfp_addr_in6_tmp->sin6_flowinfo;
				addr_in6_tmp.sin6_scope_id =
					nfp_addr_in6_tmp->sin6_scope_id;
				memcpy((unsigned char *)addr_in6_tmp.sin6_addr.s6_addr,
					(unsigned char *)nfp_addr_in6_tmp->sin6_addr.__u6_addr.__u6_addr16,
					16);

				if (*addrlen > sizeof(addr_in6_tmp))
					*addrlen = sizeof(addr_in6_tmp);

				memcpy(addr, &addr_in6_tmp, *addrlen);
				break;
			}
			default:
				return -1;
			}
		}

	} else if (libc_accept)
		accept_value = (*libc_accept)(sockfd, addr, addrlen);
	else { /* pre init*/
		LIBC_FUNCTION(accept);

		if (libc_accept)
			accept_value = (*libc_accept)(sockfd, addr, addrlen);
		else {
			accept_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Accept called on socket '%d' returned:'%d'\n",
		sockfd, accept_value);*/
	return accept_value;
}

int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	int accept_value = -1;

	if (IS_NFP_SOCKET(sockfd)) {
		union _nfp_sockaddr_storage nfp_addr_local;
		struct nfp_sockaddr *nfp_addr;
		nfp_socklen_t nfp_addrlen_local;
		nfp_socklen_t *nfp_addrlen;

		if (addr) {
			nfp_addr = (struct nfp_sockaddr *)&nfp_addr_local;

			if (!addrlen) {
				errno = EINVAL;
				return -1;
			}
			nfp_addrlen = &nfp_addrlen_local;
			nfp_addrlen_local = sizeof(nfp_addr_local);
		} else {
			nfp_addr = NULL;
			nfp_addrlen = NULL;
		}


		accept_value = nfp_accept(sockfd, nfp_addr, nfp_addrlen);
		errno = NETWRAP_ERRNO(nfp_errno);

		if (accept_value != -1 && addr) {
			switch (nfp_addr->sa_family) {
			case NFP_AF_INET:
			{
				struct sockaddr_in addr_in_tmp;
				struct nfp_sockaddr_in *nfp_addr_in_tmp =
					(struct nfp_sockaddr_in *)nfp_addr;

				addr_in_tmp.sin_family = AF_INET;
				addr_in_tmp.sin_port =
					nfp_addr_in_tmp->sin_port;
				addr_in_tmp.sin_addr.s_addr =
					nfp_addr_in_tmp->sin_addr.s_addr;

				if (*addrlen > sizeof(addr_in_tmp))
					*addrlen = sizeof(addr_in_tmp);

				memcpy(addr, &addr_in_tmp, *addrlen);
				break;
			}
			case NFP_AF_INET6:
			{
				struct sockaddr_in6 addr_in6_tmp;
				struct nfp_sockaddr_in6 *nfp_addr_in6_tmp =
					(struct nfp_sockaddr_in6 *)nfp_addr;

				addr_in6_tmp.sin6_family = AF_INET6;
				addr_in6_tmp.sin6_port =
					nfp_addr_in6_tmp->sin6_port;

				addr_in6_tmp.sin6_flowinfo =
					nfp_addr_in6_tmp->sin6_flowinfo;
				addr_in6_tmp.sin6_scope_id =
					nfp_addr_in6_tmp->sin6_scope_id;
				memcpy((unsigned char *)addr_in6_tmp.sin6_addr.s6_addr,
					(unsigned char *)nfp_addr_in6_tmp->sin6_addr.__u6_addr.__u6_addr16,
					16);

				if (*addrlen > sizeof(addr_in6_tmp))
					*addrlen = sizeof(addr_in6_tmp);

				memcpy(addr, &addr_in6_tmp, *addrlen);
				break;
			}
			default:
				return -1;
			}
		}

		if ((accept_value != -1) && (flags & SOCK_NONBLOCK)) {
			int p = 1;

			if (nfp_ioctl(accept_value, NFP_FIONBIO, &p)) {
				errno = NETWRAP_ERRNO(nfp_errno);
				nfp_close(accept_value);
				accept_value = -1;
			}
		}
	} else if (libc_accept4)
		accept_value = (*libc_accept4)(sockfd, addr, addrlen, flags);
	else { /* pre init*/
		LIBC_FUNCTION(accept4);

		if (libc_accept4)
			accept_value = (*libc_accept4)(sockfd, addr,
					addrlen, flags);
		else {
			accept_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Accept4 called on socket '%d' returned:'%d'\n",
		sockfd, accept_value);*/
	return accept_value;
}

int listen(int sockfd, int backlog)
{
	int listen_value = -1;

	if (IS_NFP_SOCKET(sockfd)) {
		listen_value = nfp_listen(sockfd, backlog);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_listen)
		listen_value = (*libc_listen)(sockfd, backlog);
	else { /* pre init*/
		LIBC_FUNCTION(listen);

		if (libc_listen)
			listen_value = (*libc_listen)(sockfd, backlog);
		else {
			listen_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Listen called on socket '%d' returns:'%d'\n",
		sockfd, listen_value);*/
	return listen_value;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int connect_value;

	if (IS_NFP_SOCKET(sockfd)) {
		union _nfp_sockaddr_storage nfp_addr_local;
		struct nfp_sockaddr *nfp_addr;
		nfp_socklen_t nfp_addrlen;

		if (!addr || addrlen < sizeof(short)) {
			errno = EINVAL;
			return -1;
		}

		nfp_addr = (struct nfp_sockaddr *)&nfp_addr_local;

		switch (addr->sa_family) {
		case AF_INET:
		{
			const struct sockaddr_in *addr_in_tmp;
			struct nfp_sockaddr_in *nfp_addr_in_tmp;

			if (addrlen < sizeof(struct sockaddr_in)) {
				errno = EINVAL;
				return -1;
			}
			addr_in_tmp = (const struct sockaddr_in *)addr;
			nfp_addr_in_tmp = (struct nfp_sockaddr_in *)nfp_addr;

			nfp_addr_in_tmp->sin_family = NFP_AF_INET;
			nfp_addr_in_tmp->sin_port = addr_in_tmp->sin_port;
			nfp_addr_in_tmp->sin_len =
				sizeof(struct nfp_sockaddr_in);
			nfp_addr_in_tmp->sin_addr.s_addr =
				addr_in_tmp->sin_addr.s_addr;

			nfp_addrlen = sizeof(struct nfp_sockaddr_in);
			break;
		}
		case AF_INET6:
		{
			const struct sockaddr_in6 *addr_in6_tmp;
			struct nfp_sockaddr_in6 *nfp_addr_in6_tmp;

			if (addrlen < sizeof(struct sockaddr_in6)) {
				errno = EINVAL;
				return -1;
			}
			addr_in6_tmp = (const struct sockaddr_in6 *)addr;
			nfp_addr_in6_tmp = (struct nfp_sockaddr_in6 *)nfp_addr;

			nfp_addr_in6_tmp->sin6_family = NFP_AF_INET6;
			nfp_addr_in6_tmp->sin6_port = addr_in6_tmp->sin6_port;
			nfp_addr_in6_tmp->sin6_flowinfo =
				addr_in6_tmp->sin6_flowinfo;
			nfp_addr_in6_tmp->sin6_scope_id =
				addr_in6_tmp->sin6_scope_id;
			nfp_addr_in6_tmp->sin6_len =
				sizeof(struct nfp_sockaddr_in6);

			memcpy((unsigned char *)nfp_addr_in6_tmp->sin6_addr.__u6_addr.__u6_addr16,
				(const unsigned char *)addr_in6_tmp->sin6_addr.s6_addr,
				16);

			nfp_addrlen = sizeof(struct nfp_sockaddr_in6);
			break;
		}

		default:
			errno = EAFNOSUPPORT;
			return -1;
		};

		connect_value = nfp_connect(sockfd,
			nfp_addr,
			nfp_addrlen);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_connect)
		connect_value = (*libc_connect)(sockfd, addr, addrlen);
	else {
		LIBC_FUNCTION(connect);

		if (libc_connect)
			connect_value = (*libc_connect)(sockfd, addr, addrlen);
		else {
			connect_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Connect called on socket '%d' returns:'%d'\n",
		sockfd, connect_value);*/
	return connect_value;
}

ssize_t read(int sockfd, void *buf, size_t len)
{
	ssize_t read_value;

	if (IS_NFP_SOCKET(sockfd)) {
		read_value = nfp_recv(sockfd, buf, (nfp_size_t)len, 0);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_read)
		read_value = (*libc_read)(sockfd, buf, len);
	else {
		LIBC_FUNCTION(read);

		if (libc_read)
			read_value = (*libc_read)(sockfd, buf, len);
		else {
			read_value = -1;
			errno = EACCES;
		}
	}

	return read_value;
}

ssize_t write(int sockfd, const void *buf, size_t len)
{
	ssize_t write_value;

	if (IS_NFP_SOCKET(sockfd)) {
		write_value = nfp_send(sockfd, buf, len, 0);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_write)
		write_value = (*libc_write)(sockfd, buf, len);
	else {
		LIBC_FUNCTION(write);
		if (libc_write)
			write_value = (*libc_write)(sockfd, buf, len);
		else {
			write_value = -1;
			errno = EACCES;
		}
	}

	return write_value;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
	ssize_t recv_value;

	if (IS_NFP_SOCKET(sockfd)) {
		int nfp_flags = 0;

		if (flags) {
			/*if (flags & MSG_CMSG_CLOEXEC)
				nfp_flags |= MSG_CMSG_CLOEXEC;*/
			if (flags & MSG_DONTWAIT)
				nfp_flags |= NFP_MSG_DONTWAIT;
			/*if (flags & MSG_ERRQUEUE)
				nfp_flags |= MSG_ERRQUEUE;*/
			if (flags & MSG_OOB)
				nfp_flags |= NFP_MSG_OOB;
			if (flags & MSG_PEEK)
				nfp_flags |= NFP_MSG_PEEK;
			if (flags & MSG_TRUNC)
				nfp_flags |= NFP_MSG_TRUNC;
			if (flags & MSG_WAITALL)
				nfp_flags |= NFP_MSG_WAITALL;
		}

		recv_value = nfp_recv(sockfd, buf, (nfp_size_t)len, nfp_flags);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_recv)
		recv_value = (*libc_recv)(sockfd, buf, len, flags);
	else { /* pre init*/
		LIBC_FUNCTION(recv);

		if (libc_recv)
			recv_value = (*libc_recv)(sockfd, buf, len, flags);
		else {
			recv_value = -1;
			errno = EACCES;
		}
	}

	return recv_value;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
	ssize_t send_value;

	if (IS_NFP_SOCKET(sockfd)) {
		int nfp_flags = 0;

		if (flags) {
			/*if (flags & MSG_CONFIRM)
				nfp_flags |= NFP_MSG_CONFIRM;*/
			if (flags & MSG_DONTROUTE)
				nfp_flags |= NFP_MSG_DONTROUTE;
			if (flags & MSG_DONTWAIT)
				nfp_flags |= NFP_MSG_DONTWAIT;
			if (flags & MSG_DONTWAIT)
				nfp_flags |= NFP_MSG_DONTWAIT;
			if (flags & MSG_EOR)
				nfp_flags |= NFP_MSG_EOR;
			/*if (flags & MSG_MORE)
				nfp_flags |= NFP_MSG_MORE;*/
			if (flags & MSG_NOSIGNAL)
				nfp_flags |= NFP_MSG_NOSIGNAL;
			if (flags & MSG_OOB)
				nfp_flags |= NFP_MSG_OOB;
		}

		send_value = nfp_send(sockfd, buf, len, nfp_flags);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_send)
		send_value = (*libc_send)(sockfd, buf, len, flags);
	else {
		LIBC_FUNCTION(send);

		if (libc_send)
			send_value = (*libc_send)(sockfd, buf, len, flags);
		else {
			send_value = -1;
			errno = EACCES;
		}
	}

	return send_value;
}


