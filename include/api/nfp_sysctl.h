/*-
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 2015, Nokia Solutions and Networks
 * Copyright (c) 2015, ENEA Software AB
 *
 * This code is derived from software contributed to Berkeley by
 * Mike Karels at Berkeley Software Design, Inc.
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
 */

#ifndef __NFP_SYSCTL_H__
#define __NFP_SYSCTL_H__

#include <stddef.h>
#include <stdint.h>
#include "nfp_queue.h"
#include "nfp_types.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Each subsystem defined by sysctl defines a list of variables
 * for that subsystem. Each name is either a node with further
 * levels defined below it, or it is a leaf of some particular
 * type given below. Each sysctl level defines a set of name/type
 * pairs to be used by sysctl(8) in manipulating the subsystem.
 */
#define NFP_CTLTYPE		0xf	/* Mask for the type */
#define	NFP_CTLTYPE_NODE	1	/* name is a node */
#define	NFP_CTLTYPE_INT	2	/* name describes an integer */
#define	NFP_CTLTYPE_STRING	3	/* name describes a string */
#define	NFP_CTLTYPE_S64	4	/* name describes a signed 64-bit number */
#define	NFP_CTLTYPE_OPAQUE	5	/* name describes a structure */
#define	NFP_CTLTYPE_STRUCT	NFP_CTLTYPE_OPAQUE	/* name describes a structure */
#define	NFP_CTLTYPE_UINT	6	/* name describes an unsigned integer */
#define	NFP_CTLTYPE_LONG	7	/* name describes a long */
#define	NFP_CTLTYPE_ULONG	8	/* name describes an unsigned long */
#define	NFP_CTLTYPE_U64		9 /* name describes an unsigned 64-bit number */
#define	NFP_CTLTYPE_PROC	10	/* name describes a procedure */

#define NFP_CTLFLAG_RD	0x80000000	/* Allow reads of variable */
#define NFP_CTLFLAG_WR	0x40000000	/* Allow writes to the variable */
#define NFP_CTLFLAG_RW	(NFP_CTLFLAG_RD|NFP_CTLFLAG_WR)
#define NFP_CTLFLAG_ANYBODY	0x10000000	/* All users can set this var */
#define NFP_CTLFLAG_SECURE	0x08000000	/* Permit set only if securelevel<=0 */
#define NFP_CTLFLAG_PRISON	0x04000000	/* Prisoned roots can fiddle */
#define NFP_CTLFLAG_DYN	0x02000000	/* Dynamic oid - can be freed */
#define NFP_CTLFLAG_SKIP	0x01000000	/* Skip this sysctl when listing */
#define NFP_CTLMASK_SECURE	0x00F00000	/* Secure level */
#define NFP_CTLFLAG_TUN	0x00080000	/* Tunable variable */
#define NFP_CTLFLAG_MPSAFE	0x00040000	/* Handler is MP safe */
#define NFP_CTLFLAG_VNET	0x00020000	/* Prisons with vnet can fiddle */
#define NFP_CTLFLAG_RDTUN	(NFP_CTLFLAG_RD|NFP_CTLFLAG_TUN)
#define	NFP_CTLFLAG_DYING	0x00010000	/* oid is being removed */
#define NFP_CTLFLAG_CAPRD	0x00008000	/* Can be read in capability mode */
#define NFP_CTLFLAG_CAPWR	0x00004000	/* Can be written in capability mode */
#define NFP_CTLFLAG_CAPRW	(NFP_CTLFLAG_CAPRD|NFP_CTLFLAG_CAPWR)

int	nfp_sysctl(const char *name, void *old, nfp_size_t *oldlenp,
		   const void *newp, nfp_size_t newlen, nfp_size_t *retval);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_SYSCTL_H__ */
