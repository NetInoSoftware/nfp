/*-
 * Copyright (c) 1982, 1985, 1986, 1988, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * Copyright (c) 2014, Nokia
 * Copyright (c) 2014, Enea Software AB
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
 *	@(#)socket.h	8.4 (Berkeley) 2/21/94
 * $FreeBSD: release/9.1.0/sys/sys/socket.h 232805 2012-03-11 00:48:54Z kib $
 */

#ifndef __NFP_SOCKET_H__
#define __NFP_SOCKET_H__

#include <odp_api.h>
#include "nfp_socket_types.h"
#include "nfp_config.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * Definitions related to sockets: types, address families, options.
 */

/*
 * Data types.
 */
#ifndef NFP__SA_FAMILY_T_DECLARED
typedef	__nfp_sa_family_t	nfp_sa_family_t;
#define	NFP__SA_FAMILY_T_DECLARED
#endif /* NFP__SA_FAMILY_T_DECLARED */

#ifndef NFP__SOCKLEN_T_DECLARED
typedef	__nfp_socklen_t	nfp_socklen_t;
#define NFP__SOCKLEN_T_DECLARED
#endif /* NFP__SOCKLEN_T_DECLARED */

#ifndef NFP__OFF_T_DECLARED
typedef	__nfp_off_t		nfp_off_t;
#define	NFP__OFF_T_DECLARED
#endif /* NFP__OFF_T_DECLARED */

/*
 * Types
 */
#define	NFP_SOCK_STREAM	1		/* stream socket */
#define	NFP_SOCK_DGRAM	2		/* datagram socket */
#define	NFP_SOCK_RAW		3		/* raw-protocol interface */
#define	NFP_SOCK_RDM		4		/* reliably-delivered message */
#define	NFP_SOCK_SEQPACKET	5		/* sequenced packet stream */
#define NFP_SOCK_EPOLL          6       /* epoll socket */

/*
 * Option flags per-socket, kept in so_options.
 */
#define	NFP_SO_DEBUG		0x00000001	/* turn on debugging info recording */
#define	NFP_SO_ACCEPTCONN	0x00000002	/* socket has had listen() */
#define	NFP_SO_REUSEADDR	0x00000004	/* allow local address reuse */
#define	NFP_SO_KEEPALIVE	0x00000008	/* keep connections alive */
#define	NFP_SO_DONTROUTE	0x00000010	/* just use interface addresses */
#define	NFP_SO_BROADCAST	0x00000020	/* permit sending of broadcast msgs */
#define	NFP_SO_USELOOPBACK	0x00000040	/* bypass hardware when possible */
#define	NFP_SO_LINGER		0x00000080	/* linger on close if data present */
#define	NFP_SO_OOBINLINE	0x00000100	/* leave received OOB data in line */
#define	NFP_SO_REUSEPORT	0x00000200	/* allow local address & port reuse */
#define	NFP_SO_TIMESTAMP	0x00000400	/* timestamp received dgram traffic */
#define	NFP_SO_NOSIGPIPE	0x00000800	/* no SIGPIPE from NFP_EPIPE */
#define	NFP_SO_ACCEPTFILTER	0x00001000	/* there is an accept filter */
#define	NFP_SO_BINTIME	0x00002000	/* timestamp received dgram traffic */
#define	NFP_SO_NO_OFFLOAD	0x00004000	/* socket cannot be offloaded */
#define	NFP_SO_NO_DDP		0x00008000	/* disable direct data placement */
#define	NFP_SO_PROMISC	0x00010000	/* socket will be used for promiscuous listen */
#define	NFP_SO_PASSIVE	0x00020000	/* socket will be used for passive reassembly */
#define	NFP_SO_PASSIVECLNT	0x00040000	/* client socket in the passive pair */
#define	NFP_SO_ALTFIB		0x00080000	/* alternate FIB is set */

/*
 * Additional options, not kept in so_options.
 */
#define	NFP_SO_SNDBUF		0x1001		/* send buffer size */
#define	NFP_SO_RCVBUF		0x1002		/* receive buffer size */
#define	NFP_SO_SNDLOWAT	0x1003		/* send low-water mark */
#define	NFP_SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define	NFP_SO_SNDTIMEO	0x1005		/* send timeout */
#define	NFP_SO_RCVTIMEO	0x1006		/* receive timeout */
#define	NFP_SO_ERROR		0x1007		/* get error status and clear */
#define	NFP_SO_TYPE		0x1008		/* get socket type */
#define	NFP_SO_LABEL		0x1009		/* socket's MAC label */
#define	NFP_SO_PEERLABEL	0x1010		/* socket's peer's MAC label */
#define	NFP_SO_LISTENQLIMIT	0x1011		/* socket's backlog limit */
#define	NFP_SO_LISTENQLEN	0x1012		/* socket's complete queue length */
#define	NFP_SO_LISTENINCQLEN	0x1013		/* socket's incomplete queue length */
#define	NFP_SO_SETFIB		0x1014		/* use this FIB to route */
#define	NFP_SO_USER_COOKIE	0x1015		/* user cookie (dummynet etc.) */
#define	NFP_SO_PROTOCOL	0x1016		/* get socket protocol (Linux name) */
#define	NFP_SO_PROTOTYPE	NFP_SO_PROTOCOL	/* alias for NFP_SO_PROTOCOL (SunOS name) */
#define NFP_SO_L2INFO		0x1017		/* PROMISCUOUS_INET MAC addrs and tags */

/*
 * Structure used for manipulating linger option.
 */
struct nfp_linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time */
};

struct accept_filter_arg {
	char	af_name[16];
	char	af_arg[256-16];
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	NFP_SOL_SOCKET	0xffff		/* options for socket level */

/*
 * Address families.
 */
#define	NFP_AF_UNSPEC	0		/* unspecified */
#define	NFP_AF_UNIX	1		/* standardized name for NFP_AF_LOCAL */
#define	NFP_AF_LOCAL	NFP_AF_UNIX
#define	NFP_AF_INET	2		/* internetwork: UDP, TCP, etc. */
#define	NFP_AF_INET6	3		/* IPv6 */
#define	NFP_AF_LINK	4		/* Link layer interface */
#define	NFP_AF_MAX	5

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct nfp_sockaddr {
	unsigned char		sa_len;		/* total length */
	nfp_sa_family_t	sa_family;	/* address family */
	char			sa_data[14];	/* actually longer; address value */
};

#define	NFP_SOCK_MAXADDRLEN	255		/* longest possible addresses */

/*
 * Structure used by kernel to pass protocol
 * information in raw sockets.
 */
struct nfp_sockproto {
	unsigned short	sp_family;		/* address family */
	unsigned short	sp_protocol;		/* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define	NFP_PF_UNSPEC	NFP_AF_UNSPEC
#define	NFP_PF_UNIX	NFP_AF_UNIX	/* backward compatibility */
#define	NFP_PF_LOCAL	NFP_AF_LOCAL	/* backward compatibility */
#define	NFP_PF_INET	NFP_AF_INET
#define	NFP_PF_INET6	NFP_AF_INET6
#define	NFP_PF_MAX	NFP_AF_MAX

/*
 * NFP_PF_ROUTE - Routing table
 *
 * Three additional levels are defined:
 *	Fourth: address family, 0 is wildcard
 *	Fifth: type of info, defined below
 *	Sixth: flag(s) to mask with for NET_RT_FLAGS
 */
#define NET_RT_DUMP	1		/* dump; may limit to a.f. */
#define NET_RT_FLAGS	2		/* by flags, e.g. RESOLVING */
#define NET_RT_IFLIST	3		/* survey interface list */
#define	NET_RT_IFMALIST	4		/* return multicast address list */
#define	NET_RT_IFLISTL	5		/* Survey interface list, using 'l'en
					 * versions of msghdr structs. */
#define	NET_RT_MAXID	6


/*
 * Maximum queue length specifiable by listen.
 */
#define	NFP_SOMAXCONN	128

/*
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */

struct nfp_iovec {
	void   *iov_base;	/* Base address. */
	nfp_size_t	iov_len;	/* Length. */
};
struct nfp_msghdr {
	void		*msg_name;		/* optional address */
	nfp_socklen_t	 msg_namelen;		/* size of address */
	struct nfp_iovec	*msg_iov;		/* scatter/gather array */
	int		 msg_iovlen;		/* # elements in msg_iov */
	void		*msg_control;		/* ancillary data, see below */
	nfp_socklen_t	 msg_controllen;	/* ancillary data buffer len */
	int		 msg_flags;		/* flags on received message */
};

#define	NFP_MSG_OOB		0x1		/* process out-of-band data */
#define	NFP_MSG_PEEK		0x2		/* peek at incoming message */
#define	NFP_MSG_DONTROUTE	0x4		/* send without using routing tables */
#define	NFP_MSG_EOR		0x8		/* data completes record */
#define	NFP_MSG_TRUNC		0x10		/* data discarded before delivery */
#define	NFP_MSG_CTRUNC	0x20		/* control data lost before delivery */
#define	NFP_MSG_WAITALL	0x40		/* wait for full request or error */
#define NFP_MSG_NOTIFICATION	0x2000         /* SCTP notification */
#define	NFP_MSG_DONTWAIT	0x80		/* this message should be nonblocking */
#define	NFP_MSG_EOF		0x100		/* data completes connection */
#define	NFP_MSG_NBIO		0x4000		/* FIONBIO mode, used by fifofs */
#define	NFP_MSG_COMPAT	0x8000		/* used in sendit() */
#define	NFP_MSG_SOCALLBCK	0x10000		/* for use by socket callbacks - nfp_soreceive (TCP) */
#define	NFP_MSG_NOSIGNAL	0x20000		/* do not generate SIGPIPE on EOF */
#define	NFP_MSG_HOLE_BREAK	0x40000		/* stop at and indicate hole boundary */

/*
 * Header for ancillary data objects in msg_control buffer.
 * Used for additional information with/about a datagram
 * not expressible by flags.  The format is a sequence
 * of message elements headed by cmsghdr structures.
 */
struct nfp_cmsghdr {
	nfp_socklen_t	cmsg_len;		/* data byte count, including hdr */
	int		cmsg_level;		/* originating protocol */
	int		cmsg_type;		/* protocol-specific type */
/* followed by	uint8_t  cmsg_data[]; */
};

/*
 * While we may have more groups than this, the cmsgcred struct must
 * be able to fit in an mbuf and we have historically supported a
 * maximum of 16 groups.
*/
#define CMGROUP_MAX 16

/*
 * Credentials structure, used to verify the identity of a peer
 * process that has sent us a message. This is allocated by the
 * peer process but filled in by the kernel. This prevents the
 * peer from lying about its identity. (Note that cmcred_groups[0]
 * is the effective GID.)
 */
struct nfp_cmsgcred {
	nfp_pid_t	cmcred_pid;		/* PID of sending process */
	nfp_uid_t	cmcred_uid;		/* real UID of sending process */
	nfp_uid_t	cmcred_euid;		/* effective UID of sending process */
	nfp_gid_t	cmcred_gid;		/* real GID of sending process */
	short	cmcred_ngroups;		/* number or groups */
	nfp_gid_t	cmcred_groups[CMGROUP_MAX];	/* groups */
};

/*
 * Socket credentials.
 */
struct nfp_sockcred {
	nfp_uid_t	sc_uid;			/* real user id */
	nfp_uid_t	sc_euid;		/* effective user id */
	nfp_gid_t	sc_gid;			/* real group id */
	nfp_gid_t	sc_egid;		/* effective group id */
	int	sc_ngroups;		/* number of supplemental groups */
	nfp_gid_t	sc_groups[1];		/* variable length */
};

/*
 * Compute size of a sockcred structure with groups.
 */
#define	NFP_SOCKCREDSIZE(ngrps) \
	(sizeof(struct nfp_sockcred) + (sizeof(nfp_gid_t) * ((ngrps) - 1)))

/* given pointer to struct nfp_cmsghdr, return pointer to data */
#define	NFP_CMSG_DATA(cmsg)		((unsigned char *)(cmsg) + \
				 _ALIGN(sizeof(struct nfp_cmsghdr)))

/* given pointer to struct nfp_cmsghdr, return pointer to next cmsghdr */
#define	NFP_CMSG_NXTHDR(mhdr, cmsg)	\
	((char *)(cmsg) == NULL ? NFP_CMSG_FIRSTHDR(mhdr) : \
	    ((char *)(cmsg) + _ALIGN(((struct nfp_cmsghdr *)(cmsg))->cmsg_len) + \
	  _ALIGN(sizeof(struct nfp_cmsghdr)) > \
	    (char *)(mhdr)->msg_control + (mhdr)->msg_controllen) ? \
	    (struct nfp_cmsghdr *)0 : \
	    (struct nfp_cmsghdr *)(void *)((char *)(cmsg) + \
	    _ALIGN(((struct nfp_cmsghdr *)(cmsg))->cmsg_len)))

/*
 * RFC 2292 requires to check msg_controllen, in case that the kernel returns
 * an empty list for some reasons.
 */
#define	NFP_CMSG_FIRSTHDR(mhdr) \
	((mhdr)->msg_controllen >= sizeof(struct nfp_cmsghdr) ? \
	 (struct nfp_cmsghdr *)(mhdr)->msg_control : \
	 (struct nfp_cmsghdr *)NULL)

/* HJo: NOTE! Architecture specific! */
#define	_ALIGNBYTES	(sizeof(register_t) - 1)
#define	_ALIGN(p)	(((uintptr_t)(p) + _ALIGNBYTES) & ~_ALIGNBYTES)

/* RFC 2292 additions */
#define	NFP_CMSG_SPACE(l)		(_ALIGN(sizeof(struct nfp_cmsghdr)) + _ALIGN(l))
#define	NFP_CMSG_LEN(l)		(_ALIGN(sizeof(struct nfp_cmsghdr)) + (l))

#define	NFP_CMSG_ALIGN(n)	_ALIGN(n)

/* "Socket"-level control message types: */
#define	NFP_SCM_RIGHTS	0x01		/* access rights (array of int) */
#define	NFP_SCM_TIMESTAMP	0x02		/* timestamp (struct timeval) */
#define	NFP_SCM_CREDS	0x03		/* process creds (struct cmsgcred) */
#define	NFP_SCM_BINTIME	0x04		/* timestamp (struct bintime) */

/*
 * 4.3 compat sockaddr, move to compat file later
 */
struct nfp_osockaddr {
	unsigned short sa_family;	/* address family */
	char	sa_data[14];		/* up to 14 bytes of direct address */
};

/*
 * 4.3-compat message header (move to compat file later).
 */
struct nfp_omsghdr {
	char	*msg_name;		/* optional address */
	int	msg_namelen;		/* size of address */
	struct	nfp_iovec *msg_iov;		/* scatter/gather array */
	int	msg_iovlen;		/* # elements in msg_iov */
	char	*msg_accrights;		/* access rights sent/received */
	int	msg_accrightslen;
};

struct nfp_icmpdata {
	uint64_t rtt;
	uint16_t seq;
	uint8_t ttl;
};

/*
 * howto arguments for shutdown(2), specified by Posix.1g.
 */
#define	NFP_SHUT_RD		0		/* shut down the reading side */
#define	NFP_SHUT_WR		1		/* shut down the writing side */
#define	NFP_SHUT_RDWR	2		/* shut down both sides */

/* we cheat and use the NFP_SHUT_XX defines for these */
#define NFP_PRU_FLUSH_RD     NFP_SHUT_RD
#define NFP_PRU_FLUSH_WR     NFP_SHUT_WR
#define NFP_PRU_FLUSH_RDWR   NFP_SHUT_RDWR


/*
 * sendfile(2) header/trailer struct
 */
struct nfp_sf_hdtr {
	struct nfp_iovec *headers;	/* pointer to an array of header struct iovec's */
	int hdr_cnt;		/* number of header nfp_iovec's */
	struct nfp_iovec *trailers;	/* pointer to an array of trailer struct iovec's */
	int trl_cnt;		/* number of trailer nfp_iovec's */
};

/*
 * Sendfile-specific flag(s)
 */
#define	NFP_SF_NODISKIO     0x00000001
#define	NFP_SF_MNOWAIT	0x00000002
#define	NFP_SF_SYNC		0x00000004

/* socket API */
int	nfp_socket(int, int, int);
int	nfp_socket_vrf(int, int, int, int);
int	nfp_accept(int, struct nfp_sockaddr *, nfp_socklen_t *);
int	nfp_bind(int, const struct nfp_sockaddr *, nfp_socklen_t);
int	nfp_connect(int, const struct nfp_sockaddr *, nfp_socklen_t);
int	nfp_listen(int, int);
int	nfp_shutdown(int, int);
int	nfp_close(int);

nfp_ssize_t	nfp_recv(int, void *, nfp_size_t, int);
nfp_ssize_t	nfp_recvfrom(int, void *, nfp_size_t, int,
			     struct nfp_sockaddr *__restrict,
			     nfp_socklen_t *__restrict);
nfp_ssize_t	nfp_recvmsg(int, struct nfp_msghdr *, int);

nfp_ssize_t	nfp_send(int, const void *, nfp_size_t, int);
nfp_ssize_t	nfp_sendto(int, const void *, nfp_size_t, int,
			   const struct nfp_sockaddr *, nfp_socklen_t);
nfp_ssize_t	nfp_sendmsg(int, const struct nfp_msghdr *, int);

int	nfp_setsockopt(int, int, int, const void *, nfp_socklen_t);
int	nfp_getsockopt(int, int, int, void *, nfp_socklen_t *);

int	nfp_getsockname(int, struct nfp_sockaddr *, nfp_socklen_t *);
int	nfp_getpeername(int, struct nfp_sockaddr *, nfp_socklen_t *);

int	nfp_ioctl(int, int, ...);

void	*nfp_udp_packet_parse(odp_packet_t, int *,
				struct nfp_sockaddr *,
				nfp_socklen_t *);
nfp_ssize_t nfp_udp_packet_sendto(int, odp_packet_t,
				  const struct nfp_sockaddr *, nfp_socklen_t);

#if 0 /* Not implemented */
int	nfp_sendfile(int, int, nfp_off_t, nfp_size_t, struct nfp_sf_hdtr *,
		     nfp_off_t *, int);

int	nfp_setfib(int);
int	nfp_sockatmark(int);
int	nfp_socketpair(int, int, int, int *);
#endif

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_SOCKET_H__ */
