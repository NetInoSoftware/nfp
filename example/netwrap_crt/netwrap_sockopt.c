/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <odp_api.h>
#include "nfp.h"
#include "netwrap_sockopt.h"
#include "netwrap_errno.h"

static int (*libc_setsockopt)(int, int, int, const void*, socklen_t);
static int (*libc_getsockopt)(int, int, int, void*, socklen_t*);

void setup_sockopt_wrappers(void)
{
	LIBC_FUNCTION(setsockopt);
	LIBC_FUNCTION(getsockopt);
}

int setsockopt(int sockfd, int level, int opt_name, const void *opt_val,
	socklen_t opt_len)
{
	int setsockopt_value;

	if (IS_NFP_SOCKET(sockfd)) {
		int nfp_level;
		int nfp_opt_name;

		if (level == SOL_SOCKET) {
			nfp_level = NFP_SOL_SOCKET;

			switch (opt_name) {
			case SO_LINGER:
				nfp_opt_name = NFP_SO_LINGER;
				break;
			case SO_DEBUG:
				nfp_opt_name = NFP_SO_DEBUG;
				break;
			case SO_KEEPALIVE:
				nfp_opt_name = NFP_SO_KEEPALIVE;
				break;
			case SO_DONTROUTE:
				nfp_opt_name = NFP_SO_DONTROUTE;
				break;
			/*case SO_USELOOPBACK:
				nfp_opt_name = NFP_SO_USELOOPBACK;
				break;*/
			case SO_BROADCAST:
				nfp_opt_name = NFP_SO_BROADCAST;
				break;
			case SO_REUSEADDR:
				nfp_opt_name = NFP_SO_REUSEADDR;
				break;
			case SO_REUSEPORT:
				nfp_opt_name = NFP_SO_REUSEPORT;
				break;
			case SO_OOBINLINE:
				nfp_opt_name = NFP_SO_OOBINLINE;
				break;
			case SO_TIMESTAMP:
				nfp_opt_name = NFP_SO_TIMESTAMP;
				break;
			/*case SO_BINTIME:
				nfp_opt_name = NFP_SO_BINTIME;
				break;
			case SO_NOSIGPIPE:
				nfp_opt_name = NFP_SO_NOSIGPIPE;
				break;
			case SO_NO_DDP:
				nfp_opt_name = NFP_SO_NO_DDP;
				break;
			case SO_NO_OFFLOAD:
				nfp_opt_name = NFP_SO_NO_OFFLOAD;
				break;
			case SO_SETFIB:
				nfp_opt_name = NFP_SO_SETFIB;
				break;
			case SO_USER_COOKIE:
				nfp_opt_name = NFP_SO_USER_COOKIE;
				break;*/
			case SO_SNDBUF:
				nfp_opt_name = NFP_SO_SNDBUF;
				break;
			case SO_RCVBUF:
				nfp_opt_name = NFP_SO_RCVBUF;
				break;
			case SO_SNDLOWAT:
				nfp_opt_name = NFP_SO_SNDLOWAT;
				break;
			case SO_RCVLOWAT:
				nfp_opt_name = NFP_SO_RCVLOWAT;
				break;
			case SO_SNDTIMEO:
				nfp_opt_name = NFP_SO_SNDTIMEO;
				break;
			case SO_RCVTIMEO:
				nfp_opt_name = NFP_SO_RCVTIMEO;
				break;
			/*case SO_LABEL:
				nfp_opt_name = NFP_SO_LABEL;
				break;*/
			default:
				errno = EOPNOTSUPP;
				return -1;
			};
		} else {
			nfp_level = level;
			nfp_opt_name = opt_name;
		}


		setsockopt_value = nfp_setsockopt(sockfd, nfp_level,
			nfp_opt_name, opt_val, opt_len);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_setsockopt)
		setsockopt_value = (*libc_setsockopt)(sockfd, level, opt_name,
			opt_val, opt_len);
	else {
		LIBC_FUNCTION(setsockopt);

		if (libc_setsockopt)
			setsockopt_value = (*libc_setsockopt)(sockfd, level,
				opt_name, opt_val, opt_len);
		else {
			setsockopt_value = -1;
			errno = EACCES;
		}
	}

	return setsockopt_value;
}


int getsockopt(int sockfd, int level, int opt_name, void *opt_val,
	socklen_t *opt_len)
{
	int getsockopt_value = -1;

	if (IS_NFP_SOCKET(sockfd)) {
		int nfp_level;
		int nfp_opt_name;

		if (level == SOL_SOCKET) {
			nfp_level = NFP_SOL_SOCKET;

			switch (opt_name) {
			case SO_LINGER:
				nfp_opt_name = NFP_SO_LINGER;
				break;
			case SO_DEBUG:
				nfp_opt_name = NFP_SO_DEBUG;
				break;
			case SO_KEEPALIVE:
				nfp_opt_name = NFP_SO_KEEPALIVE;
				break;
			case SO_DONTROUTE:
				nfp_opt_name = NFP_SO_DONTROUTE;
				break;
			/*case SO_USELOOPBACK:
				nfp_opt_name = NFP_SO_USELOOPBACK;
				break;*/
			case SO_BROADCAST:
				nfp_opt_name = NFP_SO_BROADCAST;
				break;
			case SO_REUSEADDR:
				nfp_opt_name = NFP_SO_REUSEADDR;
				break;
			case SO_REUSEPORT:
				nfp_opt_name = NFP_SO_REUSEPORT;
				break;
			case SO_OOBINLINE:
				nfp_opt_name = NFP_SO_OOBINLINE;
				break;
			case SO_ACCEPTCONN:
				nfp_opt_name = NFP_SO_ACCEPTCONN;
				break;
			case SO_TIMESTAMP:
				nfp_opt_name = NFP_SO_TIMESTAMP;
				break;
			/*case SO_BINTIME:
				nfp_opt_name = NFP_SO_BINTIME;
				break;
			case SO_NOSIGPIPE:
				nfp_opt_name = NFP_SO_NOSIGPIPE;
				break;
			case SO_SETFIB:
				nfp_opt_name = NFP_SO_SETFIB;
				break;
			case SO_USER_COOKIE:
				nfp_opt_name = NFP_SO_USER_COOKIE;
				break;*/
			case SO_SNDBUF:
				nfp_opt_name = NFP_SO_SNDBUF;
				break;
			case SO_RCVBUF:
				nfp_opt_name = NFP_SO_RCVBUF;
				break;
			case SO_SNDLOWAT:
				nfp_opt_name = NFP_SO_SNDLOWAT;
				break;
			case SO_RCVLOWAT:
				nfp_opt_name = NFP_SO_RCVLOWAT;
				break;
			case SO_SNDTIMEO:
				nfp_opt_name = NFP_SO_SNDTIMEO;
				break;
			case SO_RCVTIMEO:
				nfp_opt_name = NFP_SO_RCVTIMEO;
				break;
			/*case SO_LABEL:
				nfp_opt_name = NFP_SO_LABEL;
				break;*/
			case SO_TYPE:
				nfp_opt_name = NFP_SO_TYPE;
				break;
			case SO_PROTOCOL:
				nfp_opt_name = NFP_SO_PROTOCOL;
				break;
			case SO_ERROR:
				nfp_opt_name = NFP_SO_ERROR;
				break;
			/*case SO_LISTENQLIMIT:
				nfp_opt_name = NFP_SO_LISTENQLIMIT;
				break;
			case SO_LISTENQLEN:
				nfp_opt_name = NFP_SO_LISTENQLEN;
				break;
			case SO_LISTENINCQLEN:
				nfp_opt_name = NFP_SO_LISTENINCQLEN;
				break;*/
			default:
				errno = EOPNOTSUPP;
				return -1;
			};
		} else {
			nfp_level = level;
			nfp_opt_name = opt_name;
		}

		getsockopt_value = nfp_getsockopt(sockfd, nfp_level,
			nfp_opt_name, opt_val, opt_len);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_getsockopt)
		getsockopt_value = (*libc_getsockopt)(sockfd, level, opt_name,
			opt_val, opt_len);
	else {
		LIBC_FUNCTION(getsockopt);

		if (libc_getsockopt)
			getsockopt_value = (*libc_getsockopt)(sockfd, level,
					opt_name, opt_val, opt_len);
		else {
			getsockopt_value = -1;
			errno = EACCES;
		}
	}
	return getsockopt_value;
}
