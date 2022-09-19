/* Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/*-
 * Copyright (c) 1982, 1986, 1993
 *	The Regents of the University of California.
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
 *	@(#)udp.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: release/9.1.0/sys/netinet/udp.h 217126 2011-01-07 21:40:34Z jhb $
 */

#ifndef __NFP_UDP_H__
#define __NFP_UDP_H__

#include <stdint.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * UDP protocol header.
 * Per RFC 768, September, 1981.
 */
struct nfp_udphdr {
	uint16_t	uh_sport;		/* source port */
	uint16_t	uh_dport;		/* destination port */
	uint16_t	uh_ulen;		/* udp length */
	uint16_t	uh_sum;			/* udp checksum */
};

/*
 * User-settable options (used with setsockopt).
 */
#define NFP_UDP_ENCAP			0x01


/*
 * UDP Encapsulation of IPsec Packets options.
 */
/* Encapsulation types. */
#define NFP_UDP_ENCAP_ESPINUDP_NON_IKE	1 /* draft-ietf-ipsec-nat-t-ike-00/01 */
#define NFP_UDP_ENCAP_ESPINUDP		2 /* draft-ietf-ipsec-udp-encaps-02+ */

/* Default ESP in UDP encapsulation port. */
#define NFP_UDP_ENCAP_ESPINUDP_PORT		500

/* Maximum UDP fragment size for ESP over UDP. */
#define NFP_UDP_ENCAP_ESPINUDP_MAXFRAGLEN	552

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_UDP_H__ */