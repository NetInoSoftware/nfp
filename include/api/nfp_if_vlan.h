/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

/*-
 * Copyright 1998 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that both the above copyright notice and this
 * permission notice appear in all copies, that both the above
 * copyright notice and this permission notice appear in all
 * supporting documentation, and that the name of M.I.T. not be used
 * in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  M.I.T. makes
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THIS SOFTWARE IS PROVIDED BY M.I.T. ``AS IS''.  M.I.T. DISCLAIMS
 * ALL EXPRESS OR IMPLIED WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
 * SHALL M.I.T. BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: release/9.1.0/sys/net/if_vlan_var.h 219819 2011-03-21 09:40:01Z jeff $
 */

#ifndef __NFP_IF_VLAN_H__
#define __NFP_IF_VLAN_H__

#include "nfp_ethernet.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

struct	nfp_ether_vlan_header {
	uint8_t evl_dhost[NFP_ETHER_ADDR_LEN];
	uint8_t evl_shost[NFP_ETHER_ADDR_LEN];
	uint16_t evl_encap_proto;
	uint16_t evl_tag;
	uint16_t evl_proto;
} __attribute__((packed));

#define	NFP_EVL_VLID_MASK		0x0FFF
#define	NFP_EVL_PRI_MASK		0xE000
#define	NFP_EVL_VLANOFTAG(tag)	((tag) & NFP_EVL_VLID_MASK)
#define	NFP_EVL_PRIOFTAG(tag)	(((tag) >> 13) & 7)
#define	NFP_EVL_CFIOFTAG(tag)	(((tag) >> 12) & 1)
#define	NFP_EVL_MAKETAG(vlid, pri, cfi)				\
	((((((pri) & 7) << 1) | ((cfi) & 1)) << 12) | ((vlid) &		\
						NFP_EVL_VLID_MASK))	\

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IF_VLAN_H__ */
