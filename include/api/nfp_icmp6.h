/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/*-
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
 */

#ifndef __NFP_ICMP6_H__
#define __NFP_ICMP6_H__

#include <odp_api.h>
#include "nfp_in6.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define NFP_ICMPV6_PLD_MAXLEN	1232	/* IPV6_MMTU - sizeof(struct ip6_hdr)
					   - sizeof(struct icmp6_hdr) */

struct nfp_icmp6_hdr {
	uint8_t	icmp6_type;	/* type field */
	uint8_t	icmp6_code;	/* code field */
	uint16_t	icmp6_cksum;	/* checksum field */
	union {
		uint32_t	icmp6_un_data32[1]; /* type-specific field */
		uint16_t	icmp6_un_data16[2]; /* type-specific field */
		uint8_t	icmp6_un_data8[4];  /* type-specific field */
	} icmp6_dataun;
} __attribute__((packed));

#define nfp_icmp6_data32	icmp6_dataun.icmp6_un_data32
#define nfp_icmp6_data16	icmp6_dataun.icmp6_un_data16
#define nfp_icmp6_data8	icmp6_dataun.icmp6_un_data8
#define nfp_icmp6_pptr		nfp_icmp6_data32[0]	/* parameter prob */
#define nfp_icmp6_mtu		icmp6_data32[0]		/* packet too big */
#define nfp_icmp6_id		nfp_icmp6_data16[0]	/* echo request/reply */
#define nfp_icmp6_seq		nfp_icmp6_data16[1]	/* echo request/reply */
#define nfp_icmp6_maxdelay	icmp6_data16[0]		/* mcast group membership */

#define NFP_ICMP6_DST_UNREACH			1	/* dest unreachable, codes: */
#define NFP_ICMP6_PACKET_TOO_BIG		2	/* packet too big */
#define NFP_ICMP6_TIME_EXCEEDED		3	/* time exceeded, code: */
#define NFP_ICMP6_PARAM_PROB			4	/* ip6 header bad */

#define NFP_ICMP6_ECHO_REQUEST		128	/* echo service */
#define NFP_ICMP6_ECHO_REPLY			129	/* echo reply */
#define NFP_MLD_LISTENER_QUERY		130	/* multicast listener query */
#define NFP_MLD_LISTENER_REPORT		131	/* multicast listener report */
#define NFP_MLD_LISTENER_DONE			132	/* multicast listener done */
#define NFP_MLD_LISTENER_REDUCTION MLD_LISTENER_DONE /* RFC3542 definition */

/* RFC2292 decls */
#define NFP_ICMP6_MEMBERSHIP_QUERY		130	/* group membership query */
#define NFP_ICMP6_MEMBERSHIP_REPORT		131	/* group membership report */
#define NFP_ICMP6_MEMBERSHIP_REDUCTION	132	/* group membership termination */

/* the followings are for backward compatibility to old KAME apps. */
#define NFP_MLD6_LISTENER_QUERY		MLD_LISTENER_QUERY
#define NFP_MLD6_LISTENER_REPORT		MLD_LISTENER_REPORT
#define NFP_MLD6_LISTENER_DONE		MLD_LISTENER_DONE

#define NFP_ND_ROUTER_SOLICIT			133	/* router solicitation */
#define NFP_ND_ROUTER_ADVERT			134	/* router advertisement */
#define NFP_ND_NEIGHBOR_SOLICIT		135	/* neighbor solicitation */
#define NFP_ND_NEIGHBOR_ADVERT		136	/* neighbor advertisement */
#define NFP_ND_REDIRECT			137	/* redirect */

#define NFP_ICMP6_ROUTER_RENUMBERING		138	/* router renumbering */

#define NFP_ICMP6_WRUREQUEST			139	/* who are you request */
#define NFP_ICMP6_WRUREPLY			140	/* who are you reply */
#define NFP_ICMP6_FQDN_QUERY			139	/* FQDN query */
#define NFP_ICMP6_FQDN_REPLY			140	/* FQDN reply */
#define NFP_ICMP6_NI_QUERY			139	/* node information request */
#define NFP_ICMP6_NI_REPLY			140	/* node information reply */
#define NFP_MLDV2_LISTENER_REPORT		143	/* RFC3810 listener report */

/* The definitions below are experimental. TBA */
#define NFP_MLD_MTRACE_RESP			200	/* mtrace resp (to sender) */
#define NFP_MLD_MTRACE			201	/* mtrace messages */

#define NFP_MLD6_MTRACE_RESP			MLD_MTRACE_RESP
#define NFP_MLD6_MTRACE			MLD_MTRACE

#define NFP_ICMP6_MAXTYPE			201

#define NFP_ICMP6_DST_UNREACH_NOROUTE		0	/* no route to destination */
#define NFP_ICMP6_DST_UNREACH_ADMIN		1	/* administratively prohibited */
#define NFP_ICMP6_DST_UNREACH_NOTNEIGHBOR	2	/* not a neighbor(obsolete) */
#define NFP_ICMP6_DST_UNREACH_BEYONDSCOPE	2	/* beyond scope of source address */
#define NFP_ICMP6_DST_UNREACH_ADDR		3	/* address unreachable */
#define NFP_ICMP6_DST_UNREACH_NOPORT		4	/* port unreachable */

#define NFP_ICMP6_TIME_EXCEED_TRANSIT	0	/* ttl==0 in transit */
#define NFP_ICMP6_TIME_EXCEED_REASSEMBLY	1	/* ttl==0 in reass */

#define NFP_ICMP6_PARAMPROB_HEADER		0	/* erroneous header field */
#define NFP_ICMP6_PARAMPROB_NEXTHEADER	1	/* unrecognized next header */
#define NFP_ICMP6_PARAMPROB_OPTION		2	/* unrecognized option */

#define NFP_ICMP6_INFOMSG_MASK		0x80	/* all informational messages */

#define NFP_ICMP6_NI_SUBJ_IPV6		0	/* Query Subject is an IPv6 address */
#define NFP_ICMP6_NI_SUBJ_FQDN		1	/* Query Subject is a Domain name */
#define NFP_ICMP6_NI_SUBJ_IPV4		2	/* Query Subject is an IPv4 address */

#define NFP_ICMP6_NI_SUCCESS			0	/* node information successful reply */
#define NFP_ICMP6_NI_REFUSED			1	/* node information request is refused */
#define NFP_ICMP6_NI_UNKNOWN			2	/* unknown Qtype */

#define NFP_ICMP6_ROUTER_RENUMBERING_COMMAND  0	/* rr command */
#define NFP_ICMP6_ROUTER_RENUMBERING_RESULT   1	/* rr result */
#define NFP_ICMP6_ROUTER_RENUMBERING_SEQNUM_RESET   255	/* rr seq num reset */

/*
 * Multicast Listener Discovery
 */
struct nfp_mld_hdr {
	struct nfp_icmp6_hdr	mld_icmp6_hdr;
	struct nfp_in6_addr   mld_addr; /* multicast address */
} __attribute__((packed));

/* definitions to provide backward compatibility to old KAME applications */
#define nfp_mld6_hdr		mld_hdr
#define nfp_mld6_type		mld_type
#define nfp_mld6_code		mld_code
#define nfp_mld6_cksum	mld_cksum
#define nfp_mld6_maxdelay	mld_maxdelay
#define nfp_mld6_reserved	mld_reserved
#define nfp_mld6_addr		mld_addr

/* shortcut macro definitions */
#define nfp_mld_type		mld_icmp6_hdr.icmp6_type
#define nfp_mld_code		mld_icmp6_hdr.icmp6_code
#define nfp_mld_cksum		mld_icmp6_hdr.icmp6_cksum
#define nfp_mld_maxdelay	mld_icmp6_hdr.icmp6_data16[0]
#define nfp_mld_reserved	mld_icmp6_hdr.icmp6_data16[1]
#define nfp_mld_v2_reserved	mld_icmp6_hdr.icmp6_data16[0]
#define nfp_mld_v2_numrecs	mld_icmp6_hdr.icmp6_data16[1]

/*
 * icmp6 namelookup
 */

struct nfp_icmp6_namelookup {
	struct nfp_icmp6_hdr	icmp6_nl_hdr;
	uint8_t		icmp6_nl_nonce[8];
	int32_t		icmp6_nl_ttl;
#if 0
	uint8_t		icmp6_nl_len;
	uint8_t		icmp6_nl_name[3];
#endif
	/* could be followed by options */
} __attribute__((packed));

/*
 * icmp6 node information
 */
struct nfp_icmp6_nodeinfo {
	struct nfp_icmp6_hdr icmp6_ni_hdr;
	uint8_t icmp6_ni_nonce[8];
	/* could be followed by reply data */
} __attribute__((packed));

#define nfp_ni_type	icmp6_ni_hdr.icmp6_type
#define nfp_ni_code	icmp6_ni_hdr.icmp6_code
#define nfp_ni_cksum	icmp6_ni_hdr.icmp6_cksum
#define nfp_ni_qtype	icmp6_ni_hdr.icmp6_data16[0]
#define nfp_ni_flags	icmp6_ni_hdr.icmp6_data16[1]

#define NFP_NI_QTYPE_NOOP		0 /* NOOP  */
#define NFP_NI_QTYPE_SUPTYPES		1 /* Supported Qtypes */
#define NFP_NI_QTYPE_FQDN		2 /* FQDN (draft 04) */
#define NFP_NI_QTYPE_DNSNAME		2 /* DNS Name */
#define NFP_NI_QTYPE_NODEADDR		3 /* Node Addresses */
#define NFP_NI_QTYPE_IPV4ADDR		4 /* IPv4 Addresses */

#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_NI_SUPTYPE_FLAG_COMPRESS	0x1
#define NFP_NI_FQDN_FLAG_VALIDTTL	0x1
#elif ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_NI_SUPTYPE_FLAG_COMPRESS	0x0100
#define NFP_NI_FQDN_FLAG_VALIDTTL	0x0100
#endif

#ifdef NAME_LOOKUPS_04
#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_NI_NODEADDR_FLAG_LINKLOCAL	0x1
#define NFP_NI_NODEADDR_FLAG_SITELOCAL	0x2
#define NFP_NI_NODEADDR_FLAG_GLOBAL		0x4
#define NFP_NI_NODEADDR_FLAG_ALL		0x8
#define NFP_NI_NODEADDR_FLAG_TRUNCATE		0x10
#define NFP_NI_NODEADDR_FLAG_ANYCAST		0x20 /* just experimental. not in spec */
#elif ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_NI_NODEADDR_FLAG_LINKLOCAL	0x0100
#define NFP_NI_NODEADDR_FLAG_SITELOCAL	0x0200
#define NFP_NI_NODEADDR_FLAG_GLOBAL		0x0400
#define NFP_NI_NODEADDR_FLAG_ALL		0x0800
#define NFP_NI_NODEADDR_FLAG_TRUNCATE		0x1000
#define NFP_NI_NODEADDR_FLAG_ANYCAST		0x2000 /* just experimental. not in spec */
#endif
#else  /* draft-ietf-ipngwg-icmp-name-lookups-05 (and later?) */
#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_NI_NODEADDR_FLAG_TRUNCATE		0x1
#define NFP_NI_NODEADDR_FLAG_ALL		0x2
#define NFP_NI_NODEADDR_FLAG_COMPAT		0x4
#define NFP_NI_NODEADDR_FLAG_LINKLOCAL	0x8
#define NFP_NI_NODEADDR_FLAG_SITELOCAL	0x10
#define NFP_NI_NODEADDR_FLAG_GLOBAL		0x20
#define NFP_NI_NODEADDR_FLAG_ANYCAST		0x40 /* just experimental. not in spec */
#elif ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_NI_NODEADDR_FLAG_TRUNCATE		0x0100
#define NFP_NI_NODEADDR_FLAG_ALL		0x0200
#define NFP_NI_NODEADDR_FLAG_COMPAT		0x0400
#define NFP_NI_NODEADDR_FLAG_LINKLOCAL	0x0800
#define NFP_NI_NODEADDR_FLAG_SITELOCAL	0x1000
#define NFP_NI_NODEADDR_FLAG_GLOBAL		0x2000
#define NFP_NI_NODEADDR_FLAG_ANYCAST		0x4000 /* just experimental. not in spec */
#endif
#endif

struct nfp_ni_reply_fqdn {
	uint32_t ni_fqdn_ttl;	/* TTL */
	uint8_t ni_fqdn_namelen; /* length in octets of the FQDN */
	uint8_t ni_fqdn_name[3]; /* XXX: alignment */
} __attribute__((packed));

/*
 * Router Renumbering. as router-renum-08.txt
 */
struct nfp_icmp6_router_renum {	/* router renumbering header */
	struct nfp_icmp6_hdr	rr_hdr;
	uint8_t		rr_segnum;
	uint8_t		rr_flags;
	uint16_t	rr_maxdelay;
	uint32_t	rr_reserved;
} __attribute__((packed));

#define NFP_ICMP6_RR_FLAGS_TEST	0x80
#define NFP_ICMP6_RR_FLAGS_REQRESULT	0x40
#define NFP_ICMP6_RR_FLAGS_FORCEAPPLY	0x20
#define NFP_ICMP6_RR_FLAGS_SPECSITE	0x10
#define NFP_ICMP6_RR_FLAGS_PREVDONE	0x08

#define NFP_rr_type		rr_hdr.icmp6_type
#define NFP_rr_code		rr_hdr.icmp6_code
#define NFP_rr_cksum		rr_hdr.icmp6_cksum
#define NFP_rr_seqnum	rr_hdr.icmp6_data32[0]

struct nfp_rr_pco_match {		/* match prefix part */
	uint8_t	rpm_code;
	uint8_t	rpm_len;
	uint8_t	rpm_ordinal;
	uint8_t	rpm_matchlen;
	uint8_t	rpm_minlen;
	uint8_t	rpm_maxlen;
	uint16_t	rpm_reserved;
	struct	nfp_in6_addr	rpm_prefix;
} __attribute__((packed));

#define NFP_RPM_PCO_ADD	1
#define NFP_RPM_PCO_CHANGE	2
#define NFP_RPM_PCO_SETGLOBAL	3
#define NFP_RPM_PCO_MAX	4

struct nfp_rr_pco_use {		/* use prefix part */
	uint8_t		rpu_uselen;
	uint8_t		rpu_keeplen;
	uint8_t		rpu_ramask;
	uint8_t		rpu_raflags;
	uint32_t	rpu_vltime;
	uint32_t	rpu_pltime;
	uint32_t	rpu_flags;
	struct	nfp_in6_addr rpu_prefix;
} __attribute__((packed));
#define NFP_ICMP6_RR_PCOUSE_RAFLAGS_ONLINK	0x80
#define NFP_ICMP6_RR_PCOUSE_RAFLAGS_AUTO	0x40

#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_ICMP6_RR_PCOUSE_FLAGS_DECRVLTIME     0x80000000
#define NFP_ICMP6_RR_PCOUSE_FLAGS_DECRPLTIME     0x40000000
#elif ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_ICMP6_RR_PCOUSE_FLAGS_DECRVLTIME     0x80
#define NFP_ICMP6_RR_PCOUSE_FLAGS_DECRPLTIME     0x40
#endif

struct nfp_rr_result {		/* router renumbering result message */
	uint16_t	rrr_flags;
	uint8_t		rrr_ordinal;
	uint8_t		rrr_matchedlen;
	uint32_t	rrr_ifid;
	struct	nfp_in6_addr rrr_prefix;
} __attribute__((packed));
#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_ICMP6_RR_RESULT_FLAGS_OOB		0x0002
#define NFP_ICMP6_RR_RESULT_FLAGS_FORBIDDEN	0x0001
#elif ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_ICMP6_RR_RESULT_FLAGS_OOB		0x0200
#define NFP_ICMP6_RR_RESULT_FLAGS_FORBIDDEN	0x0100
#endif

#define NFP_ICMP6_NODEINFO_FQDNOK		0x1
#define NFP_ICMP6_NODEINFO_NODEADDROK	0x2
#define NFP_ICMP6_NODEINFO_TMPADDROK	0x4
#define NFP_ICMP6_NODEINFO_GLOBALOK		0x8

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ICMP6_H__ */
