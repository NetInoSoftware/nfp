/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/*-
 * Copyright (c) 1982, 1986, 1990, 1993
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
 *	@(#)in.h	8.3 (Berkeley) 1/3/94
 * $FreeBSD: release/9.1.0/sys/netinet/in.h 237910 2012-07-01 08:47:15Z tuexen $
 */

#ifndef __NFP_IN_H__
#define __NFP_IN_H__

#include <sys/types.h>
#include "nfp_socket.h"
#include "nfp_socket_types.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Argument structures for Protocol-Independent Multicast Source
 * Filter APIs. [RFC3678]
 */
#define	_SS_MAXSIZE	128U
#define	_SS_ALIGNSIZE	(sizeof(__int64_t))
#define	_SS_PAD1SIZE	(_SS_ALIGNSIZE - sizeof(unsigned char) - \
			    sizeof(nfp_sa_family_t))
#define	_SS_PAD2SIZE	(_SS_MAXSIZE - sizeof(unsigned char) - \
			    sizeof(nfp_sa_family_t) - _SS_PAD1SIZE - _SS_ALIGNSIZE)
struct nfp_sockaddr_storage_2 {
	unsigned char	ss_len;		/* address length */
	nfp_sa_family_t	ss_family;	/* address family */
	char		__ss_pad1[_SS_PAD1SIZE];
	__int64_t	__ss_align;	/* force desired struct alignment */
	char		__ss_pad2[_SS_PAD2SIZE];
};

/* Protocols common to RFC 1700, POSIX, and X/Open. */
#define NFP_IPPROTO_IP		0		/* dummy for IP */
#define NFP_IPPROTO_ICMP		1		/* control message protocol */
#define NFP_IPPROTO_TCP		6		/* tcp */
#define NFP_IPPROTO_UDP		17		/* user datagram protocol */

#define NFP_INADDR_ANY		(uint32_t)0x00000000
#define NFP_INADDR_BROADCAST		(uint32_t)0xffffffff	/* must be masked */

#ifndef NFP__IN_ADDR_T_DECLARED
typedef	uint32_t nfp_in_addr_t;
#define NFP__IN_ADDR_T_DECLARED
#endif

#ifndef NFP__IN_PORT_T_DECLARED
typedef	uint16_t nfp_in_port_t;
#define NFP__IN_PORT_T_DECLARED
#endif

#ifndef NFP__SA_FAMILY_T_DECLARED
typedef	uint8_t	nfp_sa_family_t;
#define NFP__SA_FAMILY_T_DECLARED
#endif

/* Internet address (a structure for historical reasons). */
#ifndef	NFP__STRUCT_IN_ADDR_DECLARED
struct nfp_in_addr {
	nfp_in_addr_t s_addr;
};
#define NFP__STRUCT_IN_ADDR_DECLARED
#endif

#ifndef NFP__SOCKLEN_T_DECLARED
typedef	__nfp_socklen_t nfp_socklen_t;
#define NFP__SOCKLEN_T_DECLARED
#endif /* NFP__SOCKLEN_T_DECLARED */

/* Socket address, internet style. */
struct nfp_sockaddr_in {
	uint8_t	sin_len;
	nfp_sa_family_t sin_family;
	nfp_in_port_t	sin_port;
	struct nfp_in_addr sin_addr;
	int8_t sin_zero[8];
};

#define	NFP_IPPROTO_RAW		255		/* raw IP packet */
#define	NFP_INET_ADDRSTRLEN		16

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, and numerous additions.
 */

/*
 * Protocols (RFC 1700)
 */
#define NFP_IPPROTO_HOPOPTS		0		/* IP6 hop-by-hop options */
#define NFP_IPPROTO_IGMP		2		/* group mgmt protocol */
#define NFP_IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define NFP_IPPROTO_IPV4		4		/* IPv4 encapsulation */
#define NFP_IPPROTO_IPIP		NFP_IPPROTO_IPV4	/* for compatibility */
#define NFP_IPPROTO_ST		7		/* Stream protocol II */
#define NFP_IPPROTO_EGP		8		/* exterior gateway protocol */
#define NFP_IPPROTO_PIGP		9		/* private interior gateway */
#define NFP_IPPROTO_RCCMON		10		/* BBN RCC Monitoring */
#define NFP_IPPROTO_NVPII		11		/* network voice protocol*/
#define NFP_IPPROTO_PUP		12		/* pup */
#define NFP_IPPROTO_ARGUS		13		/* Argus */
#define NFP_IPPROTO_EMCON		14		/* EMCON */
#define NFP_IPPROTO_XNET		15		/* Cross Net Debugger */
#define NFP_IPPROTO_CHAOS		16		/* Chaos*/
#define NFP_IPPROTO_MUX		18		/* Multiplexing */
#define NFP_IPPROTO_MEAS		19		/* DCN Measurement Subsystems */
#define NFP_IPPROTO_HMP		20		/* Host Monitoring */
#define NFP_IPPROTO_PRM		21		/* Packet Radio Measurement */
#define NFP_IPPROTO_IDP		22		/* xns idp */
#define NFP_IPPROTO_TRUNK1		23		/* Trunk-1 */
#define NFP_IPPROTO_TRUNK2		24		/* Trunk-2 */
#define NFP_IPPROTO_LEAF1		25		/* Leaf-1 */
#define NFP_IPPROTO_LEAF2		26		/* Leaf-2 */
#define NFP_IPPROTO_RDP		27		/* Reliable Data */
#define NFP_IPPROTO_IRTP		28		/* Reliable Transaction */
#define NFP_IPPROTO_TP		29		/* tp-4 w/ class negotiation */
#define NFP_IPPROTO_BLT		30		/* Bulk Data Transfer */
#define NFP_IPPROTO_NSP		31		/* Network Services */
#define NFP_IPPROTO_INP		32		/* Merit Internodal */
#define NFP_IPPROTO_SEP		33		/* Sequential Exchange */
#define NFP_IPPROTO_3PC		34		/* Third Party Connect */
#define NFP_IPPROTO_IDPR		35		/* InterDomain Policy Routing */
#define NFP_IPPROTO_XTP		36		/* XTP */
#define NFP_IPPROTO_DDP		37		/* Datagram Delivery */
#define NFP_IPPROTO_CMTP		38		/* Control Message Transport */
#define NFP_IPPROTO_TPXX		39		/* TP++ Transport */
#define NFP_IPPROTO_IL		40		/* IL transport protocol */
#define NFP_IPPROTO_IPV6		41		/* IP6 header */
#define NFP_IPPROTO_SDRP		42		/* Source Demand Routing */
#define NFP_IPPROTO_ROUTING		43		/* IP6 routing header */
#define NFP_IPPROTO_FRAGMENT		44		/* IP6 fragmentation header */
#define NFP_IPPROTO_IDRP		45		/* InterDomain Routing*/
#define NFP_IPPROTO_RSVP		46		/* resource reservation */
#define NFP_IPPROTO_GRE		47		/* General Routing Encap. */
#define NFP_IPPROTO_MHRP		48		/* Mobile Host Routing */
#define NFP_IPPROTO_BHA		49		/* BHA */
#define NFP_IPPROTO_ESP		50		/* IP6 Encap Sec. Payload */
#define NFP_IPPROTO_AH		51		/* IP6 Auth Header */
#define NFP_IPPROTO_INLSP		52		/* Integ. Net Layer Security */
#define NFP_IPPROTO_SWIPE		53		/* IP with encryption */
#define NFP_IPPROTO_NHRP		54		/* Next Hop Resolution */
#define NFP_IPPROTO_MOBILE		55		/* IP Mobility */
#define NFP_IPPROTO_TLSP		56		/* Transport Layer Security */
#define NFP_IPPROTO_SKIP		57		/* SKIP */
#define NFP_IPPROTO_ICMPV6		58		/* ICMP6 */
#define NFP_IPPROTO_NONE		59		/* IP6 no next header */
#define NFP_IPPROTO_DSTOPTS		60		/* IP6 destination option */
#define NFP_IPPROTO_AHIP		61		/* any host internal protocol */
#define NFP_IPPROTO_CFTP		62		/* CFTP */
#define NFP_IPPROTO_HELLO		63		/* "hello" routing protocol */
#define NFP_IPPROTO_SATEXPAK		64		/* SATNET/Backroom EXPAK */
#define NFP_IPPROTO_KRYPTOLAN		65		/* Kryptolan */
#define NFP_IPPROTO_RVD		66		/* Remote Virtual Disk */
#define NFP_IPPROTO_IPPC		67		/* Pluribus Packet Core */
#define NFP_IPPROTO_ADFS		68		/* Any distributed FS */
#define NFP_IPPROTO_SATMON		69		/* Satnet Monitoring */
#define NFP_IPPROTO_VISA		70		/* VISA Protocol */
#define NFP_IPPROTO_IPCV		71		/* Packet Core Utility */
#define NFP_IPPROTO_CPNX		72		/* Comp. Prot. Net. Executive */
#define NFP_IPPROTO_CPHB		73		/* Comp. Prot. HeartBeat */
#define NFP_IPPROTO_WSN		74		/* Wang Span Network */
#define NFP_IPPROTO_PVP		75		/* Packet Video Protocol */
#define NFP_IPPROTO_BRSATMON		76		/* BackRoom SATNET Monitoring */
#define NFP_IPPROTO_ND		77		/* Sun net disk proto (temp.) */
#define NFP_IPPROTO_WBMON		78		/* WIDEBAND Monitoring */
#define NFP_IPPROTO_WBEXPAK		79		/* WIDEBAND EXPAK */
#define NFP_IPPROTO_EON		80		/* ISO cnlp */
#define NFP_IPPROTO_VMTP		81		/* VMTP */
#define NFP_IPPROTO_SVMTP		82		/* Secure VMTP */
#define NFP_IPPROTO_VINES		83		/* Banyon VINES */
#define NFP_IPPROTO_TTP		84		/* TTP */
#define NFP_IPPROTO_IGP		85		/* NSFNET-IGP */
#define NFP_IPPROTO_DGP		86		/* dissimilar gateway prot. */
#define NFP_IPPROTO_TCF		87		/* TCF */
#define NFP_IPPROTO_IGRP		88		/* Cisco/GXS IGRP */
#define NFP_IPPROTO_OSPFIGP		89		/* OSPFIGP */
#define NFP_IPPROTO_SRPC		90		/* Strite RPC protocol */
#define NFP_IPPROTO_LARP		91		/* Locus Address Resoloution */
#define NFP_IPPROTO_MTP		92		/* Multicast Transport */
#define NFP_IPPROTO_AX25		93		/* AX.25 Frames */
#define NFP_IPPROTO_IPEIP		94		/* IP encapsulated in IP */
#define NFP_IPPROTO_MICP		95		/* Mobile Int.ing control */
#define NFP_IPPROTO_SCCSP		96		/* Semaphore Comm. security */
#define NFP_IPPROTO_ETHERIP		97		/* Ethernet IP encapsulation */
#define NFP_IPPROTO_ENCAP		98		/* encapsulation header */
#define NFP_IPPROTO_APES		99		/* any private encr. scheme */
#define NFP_IPPROTO_GMTP		100		/* GMTP*/
#define NFP_IPPROTO_IPCOMP		108		/* payload compression (IPComp) */
#define NFP_IPPROTO_SCTP		132		/* SCTP */
#define NFP_IPPROTO_MH		135		/* IPv6 Mobility Header */
/* 101-254: Partly Unassigned */
#define NFP_IPPROTO_PIM		103		/* Protocol Independent Mcast */
#define NFP_IPPROTO_CARP		112		/* CARP */
#define NFP_IPPROTO_PGM		113		/* PGM */
#define NFP_IPPROTO_UDPLITE		136		/* Lightweight UDP */
#define NFP_IPPROTO_PFSYNC		240		/* PFSYNC */
/* 255: Reserved */
/* BSD Private, local use, namespace incursion, no longer used */
#define NFP_IPPROTO_OLD_DIVERT	254		/* OLD divert pseudo-proto */
#define NFP_IPPROTO_MAX		256

/* last return value of *_input(), meaning "all job for this pkt is done".  */
#define NFP_IPPROTO_DONE		257

/* Only used internally, so can be outside the range of valid IP protocols. */
#define NFP_IPPROTO_DIVERT		258		/* divert pseudo-protocol */
#define NFP_IPPROTO_SEND		259		/* SeND pseudo-protocol */

/* Only used internally, so can be outside the range of valid IP protocols. */
#define NFP_IPPROTO_SP		260		/* continue processing
								on Slow Path*/
/*
 * Local port number conventions:
 *
 * When a user does a bind(2) or connect(2) with a port number of zero,
 * a non-conflicting local port address is chosen.
 * The default range is IPPORT_HIFIRSTAUTO through
 * IPPORT_HILASTAUTO, although that is settable by sysctl.
 *
 * A user may set the NFP_IPPROTO_IP option IP_PORTRANGE to change this
 * default assignment range.
 *
 * The value IP_PORTRANGE_DEFAULT causes the default behavior.
 *
 * The value IP_PORTRANGE_HIGH changes the range of candidate port numbers
 * into the "high" range.  These are reserved for client outbound connections
 * which do not want to be filtered by any firewalls.
 *
 * The value IP_PORTRANGE_LOW changes the range to the "low" are
 * that is (by convention) restricted to privileged processes.  This
 * convention is based on "vouchsafe" principles only.  It is only secure
 * if you trust the remote host to restrict these ports.
 *
 * The default range of ports and the high range can be changed by
 * sysctl(3).  (net.inet.ip.port{hi,low}{first,last}_auto)
 *
 * Changing those values has bad security implications if you are
 * using a stateless firewall that is allowing packets outside of that
 * range in order to allow transparent outgoing connections.
 *
 * Such a firewall configuration will generally depend on the use of these
 * default values.  If you change them, you may find your Security
 * Administrator looking for you with a heavy object.
 *
 * For a slightly more orthodox text view on this:
 *
 *            ftp://ftp.isi.edu/in-notes/iana/assignments/port-numbers
 *
 *    port numbers are divided into three ranges:
 *
 *                0 -  1023 Well Known Ports
 *             1024 - 49151 Registered Ports
 *            49152 - 65535 Dynamic and/or Private Ports
 *
 */

/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).         (IP_PORTRANGE_LOW)
 */
#define NFP_IPPORT_RESERVED		1024

/*
 * Default local port range, used by IP_PORTRANGE_DEFAULT
 */
#define NFP_IPPORT_EPHEMERALFIRST	10000
#define NFP_IPPORT_EPHEMERALLAST	65535

/*
 * Dynamic port range, used by IP_PORTRANGE_HIGH.
 */
#define NFP_IPPORT_HIFIRSTAUTO	49152
#define NFP_IPPORT_HILASTAUTO		65535

/*
 * Scanning for a free reserved port return a value below IPPORT_RESERVED,
 * but higher than IPPORT_RESERVEDSTART.  Traditionally the start value was
 * 512, but that conflicts with some well-known-services that firewalls may
 * have a fit if we use.
 */
#define NFP_IPPORT_RESERVEDSTART	600

#define NFP_IPPORT_MAX		65535

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define NFP_IN_CLASSA(i)		(((uint32_t)(i) & 0x80000000) == 0)
#define NFP_IN_CLASSA_NET		0xff000000
#define NFP_IN_CLASSA_NSHIFT		24
#define NFP_IN_CLASSA_HOST		0x00ffffff
#define NFP_IN_CLASSA_MAX		128

#define NFP_IN_CLASSB(i)		(((uint32_t)(i) & 0xc0000000) == 0x80000000)
#define NFP_IN_CLASSB_NET		0xffff0000
#define NFP_IN_CLASSB_NSHIFT		16
#define NFP_IN_CLASSB_HOST		0x0000ffff
#define NFP_IN_CLASSB_MAX		65536

#define NFP_IN_CLASSC(i)		(((uint32_t)(i) & 0xe0000000) == 0xc0000000)
#define NFP_IN_CLASSC_NET		0xffffff00
#define NFP_IN_CLASSC_NSHIFT		8
#define NFP_IN_CLASSC_HOST		0x000000ff

#define NFP_IN_CLASSD(i)		(((uint32_t)(i) & 0xf0000000) == 0xe0000000)
#define NFP_IN_CLASSD_NET		0xf0000000	/* These ones aren't really */
#define NFP_IN_CLASSD_NSHIFT		28		/* net and host fields, but */
#define NFP_IN_CLASSD_HOST		0x0fffffff	/* routing needn't know.    */
#define NFP_IN_MULTICAST(i)		NFP_IN_CLASSD(i)

#define NFP_IN_EXPERIMENTAL(i)	(((uint32_t)(i) & 0xf0000000) == 0xf0000000)
#define NFP_IN_BADCLASS(i)		(((uint32_t)(i) & 0xf0000000) == 0xf0000000)

#define NFP_IN_LINKLOCAL(i)		(((uint32_t)(i) & 0xffff0000) == 0xa9fe0000)
#define NFP_IN_LOOPBACK(i)		(((uint32_t)(i) & 0xff000000) == 0x7f000000)
#define NFP_IN_ZERONET(i)		(((uint32_t)(i) & 0xff000000) == 0)

#define NFP_IN_PRIVATE(i)	((((uint32_t)(i) & 0xff000000) == 0x0a000000) || \
                                 (((uint32_t)(i) & 0xfff00000) == 0xac100000) || \
                                 (((uint32_t)(i) & 0xffff0000) == 0xc0a80000))

#define NFP_IN_LOCAL_GROUP(i)	(((uint32_t)(i) & 0xffffff00) == 0xe0000000)

#define NFP_IN_ANY_LOCAL(i)		(IN_LINKLOCAL(i) || IN_LOCAL_GROUP(i))

#define NFP_INADDR_LOOPBACK		(uint32_t)0x7f000001

#define NFP_INADDR_NONE		0xffffffff		/* -1 return */

#define NFP_IN_LBROADCAST(i) ((uint32_t)(i) == NFP_INADDR_BROADCAST)


#define NFP_INADDR_UNSPEC_GROUP	(uint32_t)0xe0000000	/* 224.0.0.0 */
#define NFP_INADDR_ALLHOSTS_GROUP	(uint32_t)0xe0000001	/* 224.0.0.1 */
#define NFP_INADDR_ALLRTRS_GROUP	(uint32_t)0xe0000002	/* 224.0.0.2 */
#define NFP_INADDR_ALLRPTS_GROUP	(uint32_t)0xe0000016	/* 224.0.0.22, IGMPv3 */
#define NFP_INADDR_CARP_GROUP		(uint32_t)0xe0000012	/* 224.0.0.18 */
#define NFP_INADDR_PFSYNC_GROUP	(uint32_t)0xe00000f0	/* 224.0.0.240 */
#define NFP_INADDR_ALLMDNS_GROUP	(uint32_t)0xe00000fb	/* 224.0.0.251 */
#define NFP_INADDR_MAX_LOCAL_GROUP	(uint32_t)0xe00000ff	/* 224.0.0.255 */

#define NFP_IN_LOOPBACKNET		127			/* official! */

#define NFP_IN_RFC3021_MASK		(uint32_t)0xfffffffe

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define	NFP_IP_OPTIONS		1    /* buf/ip_opts; set/get IP options */
#define	NFP_IP_HDRINCL		2    /* int; header is included with data */
#define	NFP_IP_TOS			3    /* int; IP type of service and preced. */
#define	NFP_IP_TTL			4    /* int; IP time to live */
#define	NFP_IP_RECVOPTS		5    /* bool; receive all IP opts w/dgram */
#define	NFP_IP_RECVRETOPTS		6    /* bool; receive IP opts for response */
#define	NFP_IP_RECVDSTADDR		7    /* bool; receive IP dst addr w/dgram */
#define	NFP_IP_SENDSRCADDR		NFP_IP_RECVDSTADDR /* cmsg_type to set src addr */
#define	NFP_IP_RETOPTS		8    /* ip_opts; set/get IP options */
#define	NFP_IP_MULTICAST_IF		9    /* struct nfp_in_addr *or* struct nfp_ip_mreqn;
				      * set/get IP multicast i/f  */
#define	NFP_IP_MULTICAST_TTL	10   /* uint8_t; set/get IP multicast ttl */
#define	NFP_IP_MULTICAST_LOOP	11   /* uint8_t; set/get IP multicast loopback */
#define	NFP_IP_ADD_MEMBERSHIP	12   /* ip_mreq; add an IP group membership */
#define	NFP_IP_DROP_MEMBERSHIP	13   /* ip_mreq; drop an IP group membership */
#define	NFP_IP_MULTICAST_VIF	14   /* set/get IP mcast virt. iface */
#define	NFP_IP_RSVP_ON		15   /* enable RSVP in kernel */
#define	NFP_IP_RSVP_OFF		16   /* disable RSVP in kernel */
#define	NFP_IP_RSVP_VIF_ON		17   /* set RSVP per-vif socket */
#define	NFP_IP_RSVP_VIF_OFF		18   /* unset RSVP per-vif socket */
#define	NFP_IP_PORTRANGE		19   /* int; range to choose for unspec port */
#define	NFP_IP_RECVIF		20   /* bool; receive reception if w/dgram */
/* for IPSEC */
#define	NFP_IP_IPSEC_POLICY		21   /* int; set/get security policy */
#define	NFP_IP_FAITH		22   /* bool; accept FAITH'ed connections */

#define	NFP_IP_ONESBCAST		23   /* bool: send all-ones broadcast */
#define	NFP_IP_BINDANY		24   /* bool: allow bind to any address */

/*
 * Options for controlling the firewall and dummynet.
 * Historical options (from 40 to 64) will eventually be
 * replaced by only two options, IP_FW3 and IP_DUMMYNET3.
 */
#define	NFP_IP_FW_TABLE_ADD		40   /* add entry */
#define	NFP_IP_FW_TABLE_DEL		41   /* delete entry */
#define	NFP_IP_FW_TABLE_FLUSH	42   /* flush table */
#define	NFP_IP_FW_TABLE_GETSIZE	43   /* get table size */
#define	NFP_IP_FW_TABLE_LIST	44   /* list table contents */

#define	NFP_IP_FW3			48   /* generic ipfw v.3 sockopts */
#define	NFP_IP_DUMMYNET3		49   /* generic dummynet v.3 sockopts */

#define	NFP_IP_FW_ADD		50   /* add a firewall rule to chain */
#define	NFP_IP_FW_DEL		51   /* delete a firewall rule from chain */
#define	NFP_IP_FW_FLUSH		52   /* flush firewall rule chain */
#define	NFP_IP_FW_ZERO		53   /* clear single/all firewall counter(s) */
#define	NFP_IP_FW_GET		54   /* get entire firewall rule chain */
#define	NFP_IP_FW_RESETLOG		55   /* reset logging counters */

#define NFP_IP_FW_NAT_CFG           56   /* add/config a nat rule */
#define NFP_IP_FW_NAT_DEL           57   /* delete a nat rule */
#define NFP_IP_FW_NAT_GET_CONFIG    58   /* get configuration of a nat rule */
#define NFP_IP_FW_NAT_GET_LOG       59   /* get log of a nat rule */

#define	NFP_IP_DUMMYNET_CONFIGURE	60   /* add/configure a dummynet pipe */
#define	NFP_IP_DUMMYNET_DEL		61   /* delete a dummynet pipe from chain */
#define	NFP_IP_DUMMYNET_FLUSH	62   /* flush dummynet */
#define	NFP_IP_DUMMYNET_GET		64   /* get entire dummynet pipes */

#define	NFP_IP_RECVTTL		65   /* bool; receive IP TTL w/dgram */
#define	NFP_IP_MINTTL		66   /* minimum TTL for packet or drop */
#define	NFP_IP_DONTFRAG		67   /* don't fragment packet */
#define	NFP_IP_RECVTOS		68   /* bool; receive IP TOS w/dgram */

/* IPv4 Source Filter Multicast API [RFC3678] */
#define	NFP_IP_ADD_SOURCE_MEMBERSHIP	70   /* join a source-specific group */
#define	NFP_IP_DROP_SOURCE_MEMBERSHIP	71   /* drop a single source */
#define	NFP_IP_BLOCK_SOURCE			72   /* block a source */
#define	NFP_IP_UNBLOCK_SOURCE		73   /* unblock a source */

/* The following option is private; do not use it from user applications. */
#define	NFP_IP_MSFILTER			74   /* set/get filter list */

/* Protocol Independent Multicast API [RFC3678] */
#define	NFP_MCAST_JOIN_GROUP		80   /* join an any-source group */
#define	NFP_MCAST_LEAVE_GROUP		81   /* leave all sources for group */
#define	NFP_MCAST_JOIN_SOURCE_GROUP		82   /* join a source-specific group */
#define	NFP_MCAST_LEAVE_SOURCE_GROUP	83   /* leave a single source */
#define	NFP_MCAST_BLOCK_SOURCE		84   /* block a source */
#define	NFP_MCAST_UNBLOCK_SOURCE		85   /* unblock a source */

/* Promiscuous INET SYN filter API */
#define NFP_IP_SYNFILTER			90   /* syn_filter_optarg; attach/detach/query SYN filter */
#define NFP_IP_SYNFILTER_RESULT		91   /* syn_filter_cbarg; deliver deferred filter result */

/*
 * Defaults and limits for options
 */
#define	NFP_IP_DEFAULT_MULTICAST_TTL  1	/* normally limit m'casts to 1 hop  */
#define	NFP_IP_DEFAULT_MULTICAST_LOOP 1	/* normally hear sends if a member  */

#define	NFP_IP_MAX_SOURCE_FILTER	1024	/* XXX to be unused */

/*
 * Default resource limits for IPv4 multicast source filtering.
 * These may be modified by sysctl.
 */
#define	NFP_IP_MAX_GROUP_SRC_FILTER		512	/* sources per group */
#define	NFP_IP_MAX_SOCK_SRC_FILTER		128	/* sources per socket/group */
#define	NFP_IP_MAX_SOCK_MUTE_FILTER		128	/* XXX no longer used */

/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct nfp_ip_mreq {
	struct	nfp_in_addr imr_multiaddr;	/* IP multicast address of group */
	struct	nfp_in_addr imr_interface;	/* local IP address of interface */
};

/*
 * Modified argument structure for IP_MULTICAST_IF, obtained from Linux.
 * This is used to specify an interface index for multicast sends, as
 * the IPv4 legacy APIs do not support this (unless IP_SENDIF is available).
 */
struct nfp_ip_mreqn {
	struct	nfp_in_addr imr_multiaddr;	/* IP multicast address of group */
	struct	nfp_in_addr imr_address;	/* local IP address of interface */
	int		imr_ifindex;	/* Interface index; cast to uint32_t */
};

/*
 * Argument structure for IPv4 Multicast Source Filter APIs. [RFC3678]
 */
struct nfp_ip_mreq_source {
	struct	nfp_in_addr imr_multiaddr;	/* IP multicast address of group */
	struct	nfp_in_addr imr_sourceaddr;	/* IP address of source */
	struct	nfp_in_addr imr_interface;	/* local IP address of interface */
};

/*
 * Argument structures for Protocol-Independent Multicast Source
 * Filter APIs. [RFC3678]
 */
struct nfp_group_req {
	uint32_t		gr_interface;	/* interface index */
	struct nfp_sockaddr_storage_2	gr_group;	/* group address */
};

struct nfp_group_source_req {
	uint32_t		gsr_interface;	/* interface index */
	struct nfp_sockaddr_storage_2	gsr_group;	/* group address */
	struct nfp_sockaddr_storage_2	gsr_source;	/* source address */
};

/*
 * Filter modes; also used to represent per-socket filter mode internally.
 */
#define	NFP_MCAST_UNDEFINED	0	/* fmode: not yet defined */
#define	NFP_MCAST_INCLUDE	1	/* fmode: include these source(s) */
#define	NFP_MCAST_EXCLUDE	2	/* fmode: exclude these source(s) */


char	*nfp_inet_ntoa(struct nfp_in_addr); /* implement */

#define	nfp_in_hosteq(s, t)	((s).s_addr == (t).s_addr)
#define	nfp_in_nullhost(x)	((x).s_addr == NFP_INADDR_ANY)
#define	nfp_in_allhosts(x)	((x).s_addr == odp_cpu_to_be_32(NFP_INADDR_ALLHOSTS_GROUP))

/**
 * Calculate IPv4 header checksum.
 *
 * @param addr  Pointer to the header. This should be 2-byte aligned,
 *              otherwise performance may suffer.
 * @param ip_hl Number of 32-bit words, i.e. the value of the header
 *              length field.
 * @return Checksum.
 */
uint16_t nfp_cksum_iph(const void *addr, int ip_hl);

/**
 * Calculate checksum for IP, UDP or TCP.
 *
 * @param addr Pointer to the data. This should be 2-byte aligned,
 *             otherwise performance may suffer.
 * @param len  Number of bytes of data.
 * @return Checksum.
 */
uint16_t nfp_cksum_buffer(const void *addr, int len);

/**
 * Calculate checksum for a part of packet data.
 *
 * @param pkt The packet.
 * @param off Offset from the start of packet data.
 * @param len Number of bytes of data.
 * @return Checksum.
 */
uint16_t nfp_cksum(const odp_packet_t pkt, unsigned int off, unsigned int len);

/**
 * Calculate L4 checksum for packet.
 *
 * Construct a pseudo header and use it along with packet data to
 * calculate an L4 checksum.
 *
 * @param pkt The packet. L3 pointer must be set.
 * @return Checksum.
 */
uint16_t nfp_in4_cksum(const odp_packet_t pkt);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IN_H__*/