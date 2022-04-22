/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
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
 *	@(#)ip_var.h	8.2 (Berkeley) 1/9/95
 * $FreeBSD: release/9.1.0/sys/netinet/ip_var.h 223666 2011-06-29 10:06:58Z ae $
 */

#ifndef __NFP_IP_VAR_H__
#define __NFP_IP_VAR_H__

#include <stdint.h>
#include "nfp_in.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Overlay for ip header used by other protocols (tcp, udp).
 */
struct ipovly {
	uint8_t	ih_x1[9];		/* (unused) */
	uint8_t	ih_pr;			/* protocol */
	uint16_t	ih_len;			/* protocol length */
	struct nfp_in_addr ih_src;		/* source internet address */
	struct nfp_in_addr ih_dst;		/* destination internet address */
} __attribute__((packed));


struct	nfp_ipstat {
	uint64_t	ips_total;		/* total packets received */
	uint64_t	ips_badsum;		/* checksum bad */
	uint64_t	ips_tooshort;		/* packet too short */
	uint64_t	ips_toosmall;		/* not enough data */
	uint64_t	ips_badhlen;		/* ip header length < data size */
	uint64_t	ips_badlen;		/* ip length < ip header length */
	uint64_t	ips_fragments;		/* fragments received */
	uint64_t	ips_fragdropped;	/* frags dropped (dups, out of space) */
	uint64_t	ips_fragtimeout;	/* fragments timed out */
	uint64_t	ips_forward;		/* packets forwarded */
	uint64_t	ips_fastforward;	/* packets fast forwarded */
	uint64_t	ips_cantforward;	/* packets rcvd for unreachable dest */
	uint64_t	ips_redirectsent;	/* packets forwarded on same net */
	uint64_t	ips_noproto;		/* unknown or unsupported protocol */
	uint64_t	ips_delivered;		/* datagrams delivered to upper level*/
	uint64_t	ips_localout;		/* total ip packets generated here */
	uint64_t	ips_odropped;		/* lost packets due to nobufs, etc. */
	uint64_t	ips_reassembled;	/* total packets reassembled ok */
	uint64_t	ips_fragmented;		/* datagrams successfully fragmented */
	uint64_t	ips_ofragments;		/* output fragments created */
	uint64_t	ips_cantfrag;		/* don't fragment flag was set, etc. */
	uint64_t	ips_badoptions;		/* error in option processing */
	uint64_t	ips_noroute;		/* packets discarded due to no route */
	uint64_t	ips_badvers;		/* ip version != 4 */
	uint64_t	ips_rawout;		/* total raw ip packets generated */
	uint64_t	ips_toolong;		/* ip length > max ip packet size */
	uint64_t	ips_notmember;		/* multicasts for unregistered grps */
	uint64_t	ips_nogif;		/* no match gif found */
	uint64_t	ips_badaddr;		/* invalid address on header */
};

extern struct socket *nfp_ip_mrouter;	/* multicast routing daemon */

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IP_VAR_H__ */
