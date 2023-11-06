/* Copyright (c) 2023 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_ND6_H__
#define __NFP_ND6_H__

#include "odp_api.h"
#include "nfp_in6.h"
#include "nfp_icmp6.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* Used in kernel only */
#define NFP_ND_REDIRECT_ONLINK	0	/* redirect to an on-link node */
#define NFP_ND_REDIRECT_ROUTER	1	/* redirect to a better router */

struct nfp_nd_router_solicit {	/* router solicitation */
	struct nfp_icmp6_hdr	nd_rs_hdr;
	/* could be followed by options */
} __attribute__((packed));

#define nfp_nd_rs_type	nd_rs_hdr.icmp6_type
#define nfp_nd_rs_code	nd_rs_hdr.icmp6_code
#define nfp_nd_rs_cksum	nd_rs_hdr.icmp6_cksum
#define nfp_nd_rs_reserved	nd_rs_hdr.icmp6_data32[0]

struct nfp_nd_router_advert {	/* router advertisement */
	struct nfp_icmp6_hdr	nd_ra_hdr;
	uint32_t		nd_ra_reachable;	/* reachable time */
	uint32_t		nd_ra_retransmit;	/* retransmit timer */
	/* could be followed by options */
} __attribute__((packed));

#define nfp_nd_ra_type		nd_ra_hdr.icmp6_type
#define nfp_nd_ra_code		nd_ra_hdr.icmp6_code
#define nfp_nd_ra_cksum		nd_ra_hdr.icmp6_cksum
#define nfp_nd_ra_curhoplimit	nd_ra_hdr.nfp_icmp6_data8[0]
#define nfp_nd_ra_flags_reserved	nd_ra_hdr.nfp_icmp6_data8[1]
#define NFP_ND_RA_FLAG_MANAGED	0x80
#define NFP_ND_RA_FLAG_OTHER	0x40
#define NFP_ND_RA_FLAG_HA		0x20

/*
 * Router preference values based on draft-draves-ipngwg-router-selection-01.
 * These are non-standard definitions.
 */
#define NFP_ND_RA_FLAG_RTPREF_MASK	0x18 /* 00011000 */

#define NFP_ND_RA_FLAG_RTPREF_HIGH	0x08 /* 00001000 */
#define NFP_ND_RA_FLAG_RTPREF_MEDIUM	0x00 /* 00000000 */
#define NFP_ND_RA_FLAG_RTPREF_LOW	0x18 /* 00011000 */
#define NFP_ND_RA_FLAG_RTPREF_RSV	0x10 /* 00010000 */

#define nfp_nd_ra_router_lifetime	nd_ra_hdr.nfp_icmp6_data16[1]

struct nfp_nd_neighbor_solicit {	/* neighbor solicitation */
	struct nfp_icmp6_hdr	nd_ns_hdr;
	struct nfp_in6_addr	nd_ns_target;	/*target address */
	/* could be followed by options */
} __attribute__((packed));

#define nfp_nd_ns_type		nd_ns_hdr.icmp6_type
#define nfp_nd_ns_code		nd_ns_hdr.icmp6_code
#define nfp_nd_ns_cksum		nd_ns_hdr.icmp6_cksum
#define nfp_nd_ns_reserved	nd_ns_hdr.nfp_icmp6_data32[0]

struct nfp_nd_neighbor_advert {	/* neighbor advertisement */
	struct nfp_icmp6_hdr	nd_na_hdr;
	struct nfp_in6_addr	nd_na_target;	/* target address */
	/* could be followed by options */
} __attribute__((packed));

#define nfp_nd_na_type		nd_na_hdr.icmp6_type
#define nfp_nd_na_code		nd_na_hdr.icmp6_code
#define nfp_nd_na_cksum		nd_na_hdr.icmp6_cksum
#define nfp_nd_na_flags_reserved	nd_na_hdr.nfp_icmp6_data32[0]
#if ODP_BYTE_ORDER == ODP_LITTLE_ENDIAN
#define NFP_ND_NA_FLAG_ROUTER		0x80000000
#define NFP_ND_NA_FLAG_SOLICITED		0x40000000
#define NFP_ND_NA_FLAG_OVERRIDE		0x20000000
#else
#if ODP_BYTE_ORDER == ODP_BIG_ENDIAN
#define NFP_ND_NA_FLAG_ROUTER		0x80
#define NFP_ND_NA_FLAG_SOLICITED		0x40
#define NFP_ND_NA_FLAG_OVERRIDE		0x20
#endif
#endif

struct nfp_nd_redirect {		/* redirect */
	struct nfp_icmp6_hdr	nd_rd_hdr;
	struct nfp_in6_addr	nd_rd_target;	/* target address */
	struct nfp_in6_addr	nd_rd_dst;	/* destination address */
	/* could be followed by options */
} __attribute__((packed));

#define nfp_nd_rd_type		nd_rd_hdr.icmp6_type
#define nfp_nd_rd_code		nd_rd_hdr.icmp6_code
#define nfp_nd_rd_cksum		nd_rd_hdr.icmp6_cksum
#define nfp_nd_rd_reserved	nd_rd_hdr.icmp6_data32[0]

struct nfp_nd_opt_hdr {		/* Neighbor discovery option header */
	uint8_t	nd_opt_type;
	uint8_t	nd_opt_len;
	/* followed by option specific data*/
} __attribute__((packed));

#define NFP_ND_OPT_SOURCE_LINKADDR	1
#define NFP_ND_OPT_TARGET_LINKADDR	2
#define NFP_ND_OPT_PREFIX_INFORMATION	3
#define NFP_ND_OPT_REDIRECTED_HEADER	4
#define NFP_ND_OPT_MTU			5
#define NFP_ND_OPT_ROUTE_INFO		24	/* RFC 4191 */
#define NFP_ND_OPT_RDNSS		25	/* RFC 6106 */
#define NFP_ND_OPT_DNSSL		31	/* RFC 6106 */

struct nfp_nd_opt_prefix_info {	/* prefix information */
	uint8_t		nd_opt_pi_type;
	uint8_t		nd_opt_pi_len;
	uint8_t		nd_opt_pi_prefix_len;
	uint8_t		nd_opt_pi_flags_reserved;
	uint32_t	nd_opt_pi_valid_time;
	uint32_t	nd_opt_pi_preferred_time;
	uint32_t	nd_opt_pi_reserved2;
	struct nfp_in6_addr	nd_opt_pi_prefix;
} __attribute__((packed));

#define NFP_ND_OPT_PI_FLAG_ONLINK	0x80
#define NFP_ND_OPT_PI_FLAG_AUTO		0x40

struct nfp_nd_opt_rd_hdr {		/* redirected header */
	uint8_t		nd_opt_rh_type;
	uint8_t		nd_opt_rh_len;
	uint16_t	nd_opt_rh_reserved1;
	uint32_t	nd_opt_rh_reserved2;
	/* followed by IP header and data */
} __attribute__((packed));

struct nfp_nd_opt_mtu {		/* MTU option */
	uint8_t		nd_opt_mtu_type;
	uint8_t		nd_opt_mtu_len;
	uint16_t	nd_opt_mtu_reserved;
	uint32_t	nd_opt_mtu_mtu;
} __attribute__((packed));

struct nfp_nd_opt_route_info {	/* route info */
	uint8_t		nd_opt_rti_type;
	uint8_t		nd_opt_rti_len;
	uint8_t		nd_opt_rti_prefixlen;
	uint8_t		nd_opt_rti_flags;
	uint32_t	nd_opt_rti_lifetime;
	/* prefix follows */
} __attribute__((packed));

struct nfp_nd_opt_rdnss {		/* RDNSS option (RFC 6106) */
	uint8_t		nd_opt_rdnss_type;
	uint8_t		nd_opt_rdnss_len;
	uint16_t	nd_opt_rdnss_reserved;
	uint32_t	nd_opt_rdnss_lifetime;
	/* followed by list of recursive DNS servers */
} __attribute__((packed));

struct nfp_nd_opt_dnssl {		/* DNSSL option (RFC 6106) */
	uint8_t		nd_opt_dnssl_type;
	uint8_t		nd_opt_dnssl_len;
	uint16_t	nd_opt_dnssl_reserved;
	uint32_t	nd_opt_dnssl_lifetime;
	/* followed by list of DNS search domains */
} __attribute__((packed));

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ND6_H__ */
