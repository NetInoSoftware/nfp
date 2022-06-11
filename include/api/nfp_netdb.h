/*-
 * SPDX-License-Identifier: (BSD-3-Clause AND ISC)
 *
 * Copyright (c) 1980, 1983, 1988, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/* Copyright (c) 2022 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_NETDB_H__
#define __NFP_NETDB_H__

#include "nfp_socket.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

struct nfp_servent {
	const char    *s_name;        /* official service name */
	const char    **s_aliases;    /* alias list */
	int           s_port;         /* port # */
	const char    *s_proto;       /* protocol to use */
};

struct nfp_addrinfo {
	int     ai_flags;       /* NFP_AI_PASSIVE, NFP_AI_CANONNAME,  etc. */
	int     ai_family;      /* NFP_AF_xxx */
	int     ai_socktype;    /* NFP_SOCK_xxx */
	int     ai_protocol;    /* 0 or NFP_IPPROTO_xxx for IPv4 and IPv6 */
	nfp_socklen_t ai_addrlen;   /* length of ai_addr */
	char    *ai_canonname;  /* canonical name for hostname */
	struct  nfp_sockaddr *ai_addr;      /* binary address */
	struct  nfp_addrinfo *ai_next;      /* next structure in linked list */
};

/*
 * Flag values for nfp_getaddrinfo()
 */
#define NFP_AI_PASSIVE      0x00000001 /* get address to use bind() */
#define NFP_AI_CANONNAME    0x00000002 /* fill ai_canonname */
#define NFP_AI_NUMERICHOST  0x00000004 /* prevent host name resolution */
#define NFP_AI_NUMERICSERV  0x00000008 /* prevent service name resolution */
#define NFP_AI_ALL          0x00000100 /* IPv6 and IPv4-mapped (with
					  NFP_AI_V4MAPPED) */
#define NFP_AI_V4MAPPED_CFG 0x00000200 /* accept IPv4-mapped if supported */
#define NFP_AI_ADDRCONFIG   0x00000400 /* only if any address is assigned */
#define NFP_AI_V4MAPPED     0x00000800 /* accept IPv4-mapped IPv6 address */

/* valid flags for addrinfo (not a standard def, apps should not use it) */
#define NFP_AI_MASK \
    (NFP_AI_PASSIVE | NFP_AI_CANONNAME | NFP_AI_NUMERICHOST | \
     NFP_AI_NUMERICSERV | NFP_AI_ADDRCONFIG | NFP_AI_ALL | NFP_AI_V4MAPPED)

/* special recommended flags for getipnodebyname */
#define NFP_AI_DEFAULT      (NFP_AI_V4MAPPED_CFG | NFP_AI_ADDRCONFIG)

/*
 * Constants for nfp_getnameinfo()
 */
#define NFP_NI_MAXHOST      1025
#define NFP_NI_MAXSERV      32

/*
 * Flag values for nfp_getnameinfo()
 */
#define NFP_NI_NOFQDN       0x00000001
#define NFP_NI_NUMERICHOST  0x00000002
#define NFP_NI_NAMEREQD     0x00000004
#define NFP_NI_NUMERICSERV  0x00000008
#define NFP_NI_DGRAM        0x00000010
#define NFP_NI_NUMERICSCOPE 0x00000020

/*
 * Error return codes from nfp_gai_strerror(3), see RFC 3493.
 */
#define NFP_EAI_ADDRFAMILY   1  /* address family for hostname not supported
                                   Obsoleted on RFC 2553bis-02 */

#define NFP_EAI_AGAIN        2  /* name could not be resolved at this time */
#define NFP_EAI_BADFLAGS     3  /* flags parameter had an invalid value */
#define NFP_EAI_FAIL         4  /* non-recoverable failure in name resolution */
#define NFP_EAI_FAMILY       5  /* address family not recognized */
#define NFP_EAI_MEMORY       6  /* memory allocation failure */

#define NFP_EAI_NODATA       7  /* no address associated with hostname
                                   Obsoleted on RFC 2553bis-02 */

#define NFP_EAI_NONAME       8  /* name does not resolve */
#define NFP_EAI_SERVICE      9  /* service not recognized for socket type */
#define NFP_EAI_SOCKTYPE    10  /* intended socket type was not recognized */
#define NFP_EAI_SYSTEM      11  /* system error returned in errno */
#define NFP_EAI_BADHINTS    12  /* invalid value for hints */
#define NFP_EAI_PROTOCOL    13  /* resolved protocol is unknown */
#define NFP_EAI_OVERFLOW    14  /* argument buffer overflow */
#define NFP_EAI_MAX         15

/*
 * Scope delimit character
 */
#define NFP_SCOPE_DELIMITER '%'

const struct nfp_servent *nfp_getservbyname(const char *name,
					    const char *proto);
const struct nfp_servent *nfp_getservbyport(int port, const char *proto);

int nfp_getaddrinfo(const char *node, const char *service,
		    const struct nfp_addrinfo *hints,
		    struct nfp_addrinfo **res);
int nfp_getnameinfo(const struct nfp_sockaddr *sa, nfp_socklen_t salen,
		    char *host, nfp_size_t hostlen,
		    char *serv, nfp_size_t servlen,
		    int flags);
void nfp_freeaddrinfo(struct nfp_addrinfo *res);
const char *nfp_gai_strerror(int errcode);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_NETDB_H__ */
