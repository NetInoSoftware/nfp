/*-
 * Copyright (c) 1982, 1986, 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 2015 Nokia Solutions and Networks
 * Copyright (c) 2015 Enea Software AB
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
 *	@(#)sockio.h	8.1 (Berkeley) 3/28/94
 * $FreeBSD: release/9.1.0/sys/sys/sockio.h 223735 2011-07-03 12:22:02Z bz $
 */

#ifndef __NFP_IOCTL_H__
#define	__NFP_IOCTL_H__

#include <stddef.h>
#include "nfp_in.h"
#include "nfp_ifnet_portconf.h"
#include "nfp_socket.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Buffer with length to be used in SIOCGIFDESCR/SIOCSIFDESCR requests
 */
struct nfp_ifreq_buffer {
	nfp_size_t	length;
	void	*buffer;
};

/*
 * Interface request structure used for socket
 * nfp_ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */
struct nfp_ifreq {
	char	ifr_name[NFP_IFNAMSIZ];		/* if name, e.g. "en0" */
	union {
		struct	nfp_sockaddr ifru_addr;
		struct	nfp_sockaddr ifru_dstaddr;
		struct	nfp_sockaddr ifru_broadaddr;
		struct	nfp_ifreq_buffer ifru_buffer;
		short	ifru_flags[2];
		short	ifru_index;
		int	ifru_jid;
		int	ifru_metric;
		int	ifru_mtu;
		int	ifru_phys;
		int	ifru_media;
		char *	ifru_data;
		int	ifru_cap[2];
		uint32_t	ifru_fib;
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	ifr_buffer	ifr_ifru.ifru_buffer	/* user supplied buffer with its length */
#define	ifr_flags	ifr_ifru.ifru_flags[0]	/* flags (low 16 bits) */
#define	ifr_flagshigh	ifr_ifru.ifru_flags[1]	/* flags (high 16 bits) */
#define	ifr_jid		ifr_ifru.ifru_jid	/* jail/vnet */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define	ifr_mtu		ifr_ifru.ifru_mtu	/* mtu */
#define ifr_phys	ifr_ifru.ifru_phys	/* physical wire */
#define ifr_media	ifr_ifru.ifru_media	/* physical media */
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
#define	ifr_reqcap	ifr_ifru.ifru_cap[0]	/* requested capabilities */
#define	ifr_curcap	ifr_ifru.ifru_cap[1]	/* current capabilities */
#define	ifr_index	ifr_ifru.ifru_index	/* interface index */
#define	ifr_fib		ifr_ifru.ifru_fib	/* interface fib */
};

struct nfp_ifconf {
	int	ifc_len;		/* size of associated buffer */
	int	ifc_current_len;
	union {
		char *	ifcu_buf;
		struct nfp_ifreq *ifcu_req;
	} ifc_ifcu;
#define	ifc_buf	ifc_ifcu.ifcu_buf	/* buffer address */
#define	ifc_req	ifc_ifcu.ifcu_req	/* array of structures returned */
};

/*
 * Structure used to query names of interface cloners.
 */

struct nfp_if_clonereq {
	int	ifcr_total;		/* total cloners (out) */
	int	ifcr_count;		/* room for this many in user buffer */
	char	*ifcr_buffer;		/* buffer for cloner names */
};

/*
 * Used to lookup groups for an interface
 */
struct nfp_ifgroupreq {
	char	ifgr_name[NFP_IFNAMSIZ];
	uint32_t	ifgr_len;
	union {
		char	ifgru_group[NFP_IFNAMSIZ];
		struct	ifg_req *ifgru_groups;
	} ifgr_ifgru;
#define ifgr_group	ifgr_ifgru.ifgru_group
#define ifgr_groups	ifgr_ifgru.ifgru_groups
};

struct nfp_ifaliasreq {
	char	ifra_name[NFP_IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	nfp_sockaddr ifra_addr;
	struct	nfp_sockaddr ifra_broadaddr;
	struct	nfp_sockaddr ifra_mask;
};

struct nfp_in_aliasreq {
	char	ifra_name[NFP_IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	nfp_sockaddr_in ifra_addr;
	struct	nfp_sockaddr_in ifra_broadaddr;
#define ifra_dstaddr ifra_broadaddr
	struct	nfp_sockaddr_in ifra_mask;
};

struct nfp_in_tunreq {
	char	iftun_name[NFP_IFNAMSIZ];		/* if name, e.g. "gre1" */
	struct	nfp_sockaddr_in iftun_addr;
	struct	nfp_sockaddr_in iftun_p2p_addr;
	struct	nfp_sockaddr_in iftun_local_addr;
	struct	nfp_sockaddr_in iftun_remote_addr;
	int	iftun_vrf;
	int iftun_sp_itf_mgmt;
};

/*
 * Structure for SIOC[AGD]LIFADDR
 */
struct nfp_sockaddr_storage {
	unsigned char	ss_len;		/* address length */
	nfp_sa_family_t	ss_family;	/* address family */
};

struct nfp_if_laddrreq {
	char	iflr_name[NFP_IFNAMSIZ];
	uint32_t	flags;
#define	IFLR_PREFIX	0x8000  /* in: prefix given  out: kernel fills id */
	uint32_t	prefixlen;         /* in/out */
	struct nfp_sockaddr_storage addr;   /* in/out */
	struct nfp_sockaddr_storage dstaddr; /* out */
};

/*
 * Structure for SIOCADDRT and SIOCDELRT
 */
struct nfp_rtentry {
	struct nfp_sockaddr	rt_dst;		/* target address		*/
	struct nfp_sockaddr	rt_gateway;	/* gateway addr (RTF_GATEWAY)	*/
	struct nfp_sockaddr	rt_genmask;	/* target network mask (IP)	*/
	int			rt_vrf;
	uint16_t		rt_flags;
	int16_t			rt_metric;	/* +1 for binary compatibility!	*/
	char			*rt_dev;	/* forcing the device at add	*/
	unsigned long		rt_mtu;		/* per route MTU/Window 	*/
#define rt_mss	rt_mtu				/* Compatibility :-(            */
	unsigned long		rt_window;	/* Window clamping 		*/
	unsigned short		rt_irtt;	/* Initial RTT			*/
};

/*
 * Ioctl's have the command encoded in the lower word, and the size of
 * any in or out parameters in the upper word.  The high 3 bits of the
 * upper word are used to encode the in/out status of the parameter.
 */
#define	NFP_IOCPARM_SHIFT	13		/* number of bits for nfp_ioctl size */
#define	NFP_IOCPARM_MASK	((1 << NFP_IOCPARM_SHIFT) - 1) /* parameter length mask */
#define	NFP_IOCPARM_LEN(x)	(((x) >> 16) & NFP_IOCPARM_MASK)
#define	NFP_IOCBASECMD(x)	((x) & ~(NFP_IOCPARM_MASK << 16))
#define	NFP_IOCGROUP(x)	(((x) >> 8) & 0xff)

#define	NFP_IOCPARM_MAX	(1 << NFP_IOCPARM_SHIFT)	/* max size of nfp_ioctl */
#define	NFP_IOC_VOID	0x20000000		/* no parameters */
#define	NFP_IOC_OUT		0x40000000		/* copy out parameters */
#define	NFP_IOC_IN		0x80000000		/* copy in parameters */
#define	NFP_IOC_INOUT	(NFP_IOC_IN|NFP_IOC_OUT)
#define	NFP_IOC_DIRMASK	(NFP_IOC_VOID|NFP_IOC_OUT|NFP_IOC_IN)

#define	_NFP_IOC(inout,group,num,len)	\
	((unsigned long)((inout) | (((len) & NFP_IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define	_NFP_IO(g,n)	_NFP_IOC(NFP_IOC_VOID,	(g), (n), 0)
#define	_NFP_IOWINT(g,n)	_NFP_IOC(NFP_IOC_VOID,	(g), (n), sizeof(int))
#define	_NFP_IOR(g,n,t)	_NFP_IOC(NFP_IOC_OUT,	(g), (n), sizeof(t))
#define	_NFP_IOW(g,n,t)	_NFP_IOC(NFP_IOC_IN,	(g), (n), sizeof(t))
/* this should be _IORW, but stdio got there first */
#define	_NFP_IOWR(g,n,t)	_NFP_IOC(NFP_IOC_INOUT,	(g), (n), sizeof(t))

#define	NFP_FIONREAD		 _NFP_IOR('f', 127, int)			/* get # bytes to read */
#define	NFP_FIONBIO		 _NFP_IOW('f', 126, int)			/* set/clear non-blocking i/o */
#define	NFP_FIOASYNC		 _NFP_IOW('f', 125, int)			/* set/clear async i/o */
#define	NFP_FIONWRITE		 _NFP_IOR('f', 119, int)			/* get # bytes (yet) to write */
#define	NFP_FIONSPACE		 _NFP_IOR('f', 118, int)			/* get space in send queue */

#define	NFP_SIOCATMARK	 _NFP_IOR('s',  7, int)			/* at oob mark? */

#define	NFP_SIOCADDRT	 	 _NFP_IOW('r', 10, struct nfp_rtentry)	/* add route */
#define	NFP_SIOCDELRT	 	 _NFP_IOW('r', 11, struct nfp_rtentry)	/* delete route */

#define	NFP_SIOCSIFADDR	 _NFP_IOW('i', 12, struct nfp_ifreq)	/* set ifnet address */
#define	NFP_SIOCGIFADDR	_NFP_IOWR('i', 33, struct nfp_ifreq)	/* get ifnet address */
#define	NFP_SIOCSIFDSTADDR	 _NFP_IOW('i', 14, struct nfp_ifreq)	/* set p-p address */
#define	NFP_SIOCGIFDSTADDR	_NFP_IOWR('i', 34, struct nfp_ifreq)	/* get p-p address */
#define	NFP_OSIOCGIFBRDADDR	_NFP_IOWR('i', 18, struct nfp_ifreq)	/* get broadcast addr */
#define	NFP_SIOCGIFBRDADDR	_NFP_IOWR('i', 35, struct nfp_ifreq)	/* get broadcast addr */
#define	NFP_SIOCSIFBRDADDR	 _NFP_IOW('i', 19, struct nfp_ifreq)	/* set broadcast addr */
#define	NFP_OSIOCGIFCONF	_NFP_IOWR('i', 20, struct nfp_ifconf)	/* get ifnet list */
#define	NFP_SIOCGIFCONF	_NFP_IOWR('i', 36, struct nfp_ifconf)	/* get ifnet list */
#define	NFP_SIOCGIFNETMASK	_NFP_IOWR('i', 37, struct nfp_ifreq)	/* get net addr mask */
#define	NFP_SIOCSIFNETMASK	 _NFP_IOW('i', 22, struct nfp_ifreq)	/* set net addr mask */
#define	NFP_SIOCDIFADDR	 _NFP_IOW('i', 25, struct nfp_ifreq)	/* delete IF addr */
#define	NFP_SIOCAIFADDR	 _NFP_IOW('i', 26, struct nfp_ifaliasreq)	/* add/chg IF alias */
#define	NFP_SIOCALIFADDR	 _NFP_IOW('i', 27, struct nfp_if_laddrreq)	/* add IF addr */
#define	NFP_SIOCGLIFADDR	_NFP_IOWR('i', 28, struct nfp_if_laddrreq)	/* get IF addr */
#define	NFP_SIOCDLIFADDR	 _NFP_IOW('i', 29, struct nfp_if_laddrreq)	/* delete IF addr */
#define	NFP_SIOCGIFFIB	_NFP_IOWR('i', 92, struct nfp_ifreq)	/* get IF fib */
#define	NFP_SIOCSIFFIB	 _NFP_IOW('i', 93, struct nfp_ifreq)	/* set IF fib */
#define	NFP_SIOCGIFVRF NFP_SIOCGIFFIB
#define	NFP_SIOCSIFVRF NFP_SIOCSIFFIB
#define	NFP_SIOCIFCREATE	_NFP_IOWR('i', 122, struct nfp_ifreq)	/* create clone if */
#define	NFP_SIOCIFCREATE2	_NFP_IOWR('i', 124, struct nfp_ifreq)	/* create clone if */
#define	NFP_SIOCIFDESTROY	 _NFP_IOW('i', 121, struct nfp_ifreq)	/* destroy clone if */
#define	NFP_SIOCIFGCLONERS	_NFP_IOWR('i', 120, struct nfp_if_clonereq) /* get cloners */
#define	NFP_SIOCGIFGMEMB	_NFP_IOWR('i', 138, struct nfp_ifgroupreq)	/* get members */
#define	NFP_SIOCSIFTUN	 _NFP_IOW('i', 139, struct nfp_in_tunreq)	/* set tunnel */
#define	NFP_SIOCGIFTUN	_NFP_IOWR('i', 140, struct nfp_in_tunreq)	/* get tunnel */

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_IOCTL_H__ */
