/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_TYPES_H__
#define __NFP_TYPES_H__

#include <stdint.h>
#include "nfp_queue.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/**
 * Result of processing a packet. Indicates what, if anything, the
 * caller should do with the packet.
 */
enum nfp_return_code {
	/**
	 * The caller may continue processing the packet, and should
	 * eventually free it.
	 */
	NFP_PKT_CONTINUE = 0,
	/**
	 * The packet has been processed and may already have been
	 * freed. The caller should not use the packet any further and
	 * should not free it.
	 */
	NFP_PKT_PROCESSED,
	/**
	 * The packet is dropped. The caller should free the packet.
	 */
	NFP_PKT_DROP
};

struct nfp_nh_entry {
	uint32_t flags;
	uint32_t gw;
	uint16_t port;
	uint16_t vlan;
	uint32_t arp_ent_idx;
};

struct pkt6_entry;
NFP_SLIST_HEAD(pkt6_list, pkt6_entry);

struct nfp_nh6_entry {
	uint32_t flags;
	uint8_t  gw[16];
	uint16_t port;
	uint16_t vlan;
	uint8_t  mac[6];
	struct pkt6_list pkt6_hold;
};

typedef long		__nfp_suseconds_t;	/* microseconds (signed) */
typedef unsigned int	__nfp_useconds_t;	/* microseconds (unsigned) */
typedef int		__nfp_cpuwhich_t;	/* which parameter for cpuset.*/
typedef int		__nfp_cpulevel_t;	/* level parameter for cpuset.*/
typedef int		__nfp_cpusetid_t;	/* cpuset identifier. */
typedef uint32_t	__nfp_gid_t;
typedef uint32_t	__nfp_pid_t;
typedef uint32_t	__nfp_uid_t;
typedef uint32_t	__nfp_size_t;
typedef int32_t	__nfp_ssize_t;

#ifndef NFP__GID_T_DECLARED
typedef	__nfp_gid_t		nfp_gid_t;
#define	NFP__GID_T_DECLARED
#endif /* NFP__GID_T_DECLARED */

#ifndef NFP__PID_T_DECLARED
typedef	__nfp_pid_t		nfp_pid_t;
#define	NFP__PID_T_DECLARED
#endif /* NFP__PID_T_DECLARED */

#ifndef NFP__UID_T_DECLARED
typedef	__nfp_uid_t		nfp_uid_t;
#define	NFP__UID_T_DECLARED
#endif /*NFP__UID_T_DECLARED*/

#ifndef NFP__SSIZE_T_DECLARED
typedef	__nfp_ssize_t		nfp_ssize_t;
#define	NFP__SSIZE_T_DECLARED
#endif /* NFP__SSIZE_T_DECLARED */

#ifndef NFP__SIZE_T_DECLARED
typedef	__nfp_size_t		nfp_size_t;
#define	NFP__SIZE_T_DECLARED
#endif /* NFP__SIZE_T_DECLARED */

#ifndef NFP_MAXHOSTNAMELEN
#define NFP_MAXHOSTNAMELEN  64
#endif /* NFP_MAXHOSTNAMELEN */

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_TYPES_H__ */
