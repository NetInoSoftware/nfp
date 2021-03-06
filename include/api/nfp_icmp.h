/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

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
 *	@(#)ip_icmp.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: release/9.1.0/sys/netinet/ip_icmp.h 207369 2010-04-29 11:52:42Z bz $
 */

#ifndef __NFP_ICMP_H__
#define __NFP_ICMP_H__

#include "nfp_ip.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Interface Control Message Protocol Definitions.
 * Per RFC 792, September 1981.
 */

/*
 * Internal of an ICMP Router Advertisement
 */
struct nfp_icmp_ra_addr {
	uint32_t ira_addr;
	uint32_t ira_preference;
};

/*
 * Structure of an icmp header.
 */
struct nfp_icmphdr {
	uint8_t	icmp_type;		/* type of message, see below */
	uint8_t	icmp_code;		/* type sub code */
	uint16_t	icmp_cksum;		/* ones complement cksum of struct */
};

/*
 * Structure of an icmp packet.
 *
 * XXX: should start with a struct icmphdr.
 */
struct nfp_icmp {
	uint8_t	icmp_type;		/* type of message, see below */
	uint8_t	icmp_code;		/* type sub code */
	uint16_t	icmp_cksum;		/* ones complement cksum of struct */
	union {
		uint8_t ih_pptr;			/* ICMP_PARAMPROB */
		struct nfp_in_addr ih_gwaddr;	/* ICMP_REDIRECT */
		struct nfp_ih_idseq {
			uint16_t	icd_id;	/* network format */
			uint16_t	icd_seq; /* network format */
		} ih_idseq;
		int ih_void;

		/* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
		struct nfp_ih_pmtu {
			uint16_t ipm_void;	/* network format */
			uint16_t ipm_nextmtu;	/* network format */
		} ih_pmtu;

		struct nfp_ih_rtradv {
			uint8_t irt_num_addrs;
			uint8_t irt_wpa;
			uint16_t irt_lifetime;
		} ih_rtradv;
	} icmp_hun;
#define nfp_icmp_pptr		icmp_hun.ih_pptr
#define nfp_icmp_gwaddr	icmp_hun.ih_gwaddr
#define nfp_icmp_id		icmp_hun.ih_idseq.icd_id
#define nfp_icmp_seq		icmp_hun.ih_idseq.icd_seq
#define nfp_icmp_void		icmp_hun.ih_void
#define nfp_icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define nfp_icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define nfp_icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define nfp_icmp_wpa		icmp_hun.ih_rtradv.irt_wpa
#define nfp_icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
	union {
		struct id_ts {			/* ICMP Timestamp */
			/*
			 * The next 3 fields are in network format,
			 * milliseconds since 00:00 GMT
			 */
			uint32_t its_otime;	/* Originate */
			uint32_t its_rtime;	/* Receive */
			uint32_t its_ttime;	/* Transmit */
		} id_ts;
		struct nfp_id_ip  {
			struct nfp_ip idi_ip;
			/* options and then 64 bits of data */
		} id_ip;
		struct nfp_icmp_ra_addr id_radv;
		uint32_t id_mask;
		char	id_data[1];
	} icmp_dun;
#define nfp_icmp_otime	icmp_dun.id_ts.its_otime
#define nfp_icmp_rtime	icmp_dun.id_ts.its_rtime
#define nfp_icmp_ttime	icmp_dun.id_ts.its_ttime
#define nfp_icmp_ip		icmp_dun.id_ip.idi_ip
#define nfp_icmp_radv		icmp_dun.id_radv
#define nfp_icmp_mask		icmp_dun.id_mask
#define nfp_icmp_data		icmp_dun.id_data
};

/*
 * Lower bounds on packet lengths for various types.
 * For the error advice packets must first insure that the
 * packet is large enough to contain the returned ip header.
 * Only then can we do the check to see if 64 bits of packet
 * data have been returned, since we need to check the returned
 * ip header length.
 */
#define NFP_ICMP_MINLEN	8				/* abs minimum */
#define NFP_ICMP_TSLEN	(8 + 3 * sizeof (uint32_t))	/* timestamp */
#define NFP_ICMP_MASKLEN	12				/* address mask */
#define NFP_ICMP_ADVLENMIN	(8 + sizeof (struct nfp_ip) + 8)	/* min */
#define NFP_ICMP_ADVLEN(p)	(8 + ((p)->nfp_icmp_ip.ip_hl << 2) + 8)
	/* N.B.: must separately check that ip_hl >= 5 */

/*
 * Definition of type and code field values.
 */
#define NFP_ICMP_ECHOREPLY			0		/* echo reply */
#define NFP_ICMP_UNREACH			3		/* dest unreachable, codes: */
#define NFP_ICMP_UNREACH_NET			0		/* bad net */
#define NFP_ICMP_UNREACH_HOST			1		/* bad host */
#define NFP_ICMP_UNREACH_PROTOCOL		2		/* bad protocol */
#define NFP_ICMP_UNREACH_PORT			3		/* bad port */
#define NFP_ICMP_UNREACH_NEEDFRAG		4		/* IP_DF caused drop */
#define NFP_ICMP_UNREACH_SRCFAIL		5		/* src route failed */
#define NFP_ICMP_UNREACH_NET_UNKNOWN		6		/* unknown net */
#define NFP_ICMP_UNREACH_HOST_UNKNOWN	7		/* unknown host */
#define NFP_ICMP_UNREACH_ISOLATED		8		/* src host isolated */
#define NFP_ICMP_UNREACH_NET_PROHIB		9		/* prohibited access */
#define NFP_ICMP_UNREACH_HOST_PROHIB		10		/* ditto */
#define NFP_ICMP_UNREACH_TOSNET		11		/* bad tos for net */
#define NFP_ICMP_UNREACH_TOSHOST		12		/* bad tos for host */
#define NFP_ICMP_UNREACH_FILTER_PROHIB	13		/* admin prohib */
#define NFP_ICMP_UNREACH_HOST_PRECEDENCE	14		/* host prec vio. */
#define NFP_ICMP_UNREACH_PRECEDENCE_CUTOFF	15		/* prec cutoff */
#define NFP_ICMP_SOURCEQUENCH			4		/* packet lost, slow down */
#define NFP_ICMP_REDIRECT			5		/* shorter route, codes: */
#define NFP_ICMP_REDIRECT_NET			0		/* for network */
#define NFP_ICMP_REDIRECT_HOST		1		/* for host */
#define NFP_ICMP_REDIRECT_TOSNET		2		/* for tos and net */
#define NFP_ICMP_REDIRECT_TOSHOST		3		/* for tos and host */
#define NFP_ICMP_ALTHOSTADDR			6		/* alternate host address */
#define NFP_ICMP_ECHO				8		/* echo service */
#define NFP_ICMP_ROUTERADVERT			9		/* router advertisement */
#define NFP_ICMP_ROUTERADVERT_NORMAL		0		/* normal advertisement */
#define NFP_ICMP_ROUTERADVERT_NOROUTE_COMMON	16		/* selective routing */
#define NFP_ICMP_ROUTERSOLICIT		10		/* router solicitation */
#define NFP_ICMP_TIMXCEED			11		/* time exceeded, code: */
#define NFP_ICMP_TIMXCEED_INTRANS		0		/* ttl==0 in transit */
#define NFP_ICMP_TIMXCEED_REASS		1		/* ttl==0 in reass */
#define NFP_ICMP_PARAMPROB			12		/* ip header bad */
#define NFP_ICMP_PARAMPROB_ERRATPTR		0		/* error at param ptr */
#define NFP_ICMP_PARAMPROB_OPTABSENT		1		/* req. opt. absent */
#define NFP_ICMP_PARAMPROB_LENGTH		2		/* bad length */
#define NFP_ICMP_TSTAMP			13		/* timestamp request */
#define NFP_ICMP_TSTAMPREPLY			14		/* timestamp reply */
#define NFP_ICMP_IREQ				15		/* information request */
#define NFP_ICMP_IREQREPLY			16		/* information reply */
#define NFP_ICMP_MASKREQ			17		/* address mask request */
#define NFP_ICMP_MASKREPLY			18		/* address mask reply */
#define NFP_ICMP_TRACEROUTE			30		/* traceroute */
#define NFP_ICMP_DATACONVERR			31		/* data conversion error */
#define NFP_ICMP_MOBILE_REDIRECT		32		/* mobile host redirect */
#define NFP_ICMP_IPV6_WHEREAREYOU		33		/* IPv6 where-are-you */
#define NFP_ICMP_IPV6_IAMHERE			34		/* IPv6 i-am-here */
#define NFP_ICMP_MOBILE_REGREQUEST		35		/* mobile registration req */
#define NFP_ICMP_MOBILE_REGREPLY		36		/* mobile registration reply */
#define NFP_ICMP_SKIP				39		/* SKIP */
#define NFP_ICMP_PHOTURIS			40		/* Photuris */
#define NFP_ICMP_PHOTURIS_UNKNOWN_INDEX	1		/* unknown sec index */
#define NFP_ICMP_PHOTURIS_AUTH_FAILED		2		/* auth failed */
#define NFP_ICMP_PHOTURIS_DECRYPT_FAILED	3		/* decrypt failed */

#define NFP_ICMP_MAXTYPE			40

#define NFP_ICMP_INFOTYPE(type)                                       \
    ((type) == NFP_ICMP_ECHOREPLY    || (type) == NFP_ICMP_ECHO ||  \
     (type) == NFP_ICMP_ROUTERADVERT || (type) == NFP_ICMP_ROUTERSOLICIT || \
     (type) == NFP_ICMP_TSTAMP       || (type) == NFP_ICMP_TSTAMPREPLY || \
     (type) == NFP_ICMP_IREQ         || (type) == NFP_ICMP_IREQREPLY || \
     (type) == NFP_ICMP_MASKREQ      || (type) == NFP_ICMP_MASKREPLY)


#define BANDLIM_UNLIMITED -1
#define BANDLIM_ICMP_UNREACH 0
#define BANDLIM_ICMP_ECHO 1
#define BANDLIM_ICMP_TSTAMP 2
#define BANDLIM_RST_CLOSEDPORT 3 /* No connection, and no listeners */
#define BANDLIM_RST_OPENPORT 4   /* No connection, listener */
#define BANDLIM_ICMP6_UNREACH 5
#define BANDLIM_SCTP_OOTB 6
#define BANDLIM_MAX 6

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ICMP_H__ */
