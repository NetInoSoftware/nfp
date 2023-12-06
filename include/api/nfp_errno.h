/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 */

/**
 * @file
 *
 * @brief NFP error values (errno).
 */


#ifndef __NFP_ERRNO_H__
#define __NFP_ERRNO_H__

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define	NFP_EPERM		1	/**< Operation not permitted */
#define	NFP_ENOENT		2	/**< No such file or directory */
#define	NFP_ESRCH		3	/**< No such process */
#define	NFP_EINTR		4	/**< Interrupted system call */
#define	NFP_EIO			5	/**< Input/output error */
#define	NFP_ENXIO		6	/**< Device not configured */
#define	NFP_E2BIG		7	/**< Argument list too long */
#define	NFP_ENOEXEC		8	/**< Exec format error */
#define	NFP_EBADF		9	/**< Bad file descriptor */
#define	NFP_ECHILD		10	/**< No child processes */
#define	NFP_EDEADLK		11	/**< Resource deadlock avoided */
					/**< 11 was NFP_EAGAIN */
#define	NFP_ENOMEM		12	/**< Cannot allocate memory */
#define	NFP_EACCES		13	/**< Permission denied */
#define	NFP_EFAULT		14	/**< Bad address */

#define	NFP_ENOTBLK		15	/**< Block device required */

#define	NFP_EBUSY		16	/**< Device busy */
#define	NFP_EEXIST		17	/**< File exists */
#define	NFP_EXDEV		18	/**< Cross-device link */
#define	NFP_ENODEV		19	/**< Operation not supported by device */
#define	NFP_ENOTDIR		20	/**< Not a directory */
#define	NFP_EISDIR		21	/**< Is a directory */
#define	NFP_EINVAL		22	/**< Invalid argument */
#define	NFP_ENFILE		23	/**< Too many open files in system */
#define	NFP_EMFILE		24	/**< Too many open files */
#define	NFP_ENOTTY		25	/**< Inappropriate ioctl for device */

#define	NFP_ETXTBSY		26	/**< Text file busy */

#define	NFP_EFBIG		27	/**< File too large */
#define	NFP_ENOSPC		28	/**< No space left on device */
#define	NFP_ESPIPE		29	/**< Illegal seek */
#define	NFP_EROFS		30	/**< Read-only filesystem */
#define	NFP_EMLINK		31	/**< Too many links */
#define	NFP_EPIPE		32	/**< Broken pipe */

/* math software */
#define	NFP_EDOM		33	/**< Numerical argument out of domain */
#define	NFP_ERANGE		34	/**< Result too large */

/* non-blocking and interrupt i/o */
#define	NFP_EAGAIN		35	/**< Resource temporarily unavailable */

#define	NFP_EWOULDBLOCK	NFP_EAGAIN	/**< Operation would block */
#define	NFP_EINPROGRESS		36	/**< Operation now in progress */
#define	NFP_EALREADY		37	/**< Operation already in progress */

/* ipc/network software -- argument errors */
#define	NFP_ENOTSOCK		38	/**< Socket operation on non-socket */
#define	NFP_EDESTADDRREQ	39	/**< Destination address required */
#define	NFP_EMSGSIZE		40	/**< Message too long */
#define	NFP_EPROTOTYPE		41	/**< Protocol wrong type for socket */
#define	NFP_ENOPROTOOPT		42	/**< Protocol not available */
#define	NFP_EPROTONOSUPPORT	43	/**< Protocol not supported */
#define	NFP_ESOCKTNOSUPPORT	44	/**< Socket type not supported */
#define	NFP_EOPNOTSUPP		45	/**< Operation not supported */
#define	NFP_ENOTSUP		NFP_EOPNOTSUPP	/**< Operation not supported */
#define	NFP_EPFNOSUPPORT	46	/**< Protocol family not supported */
#define	NFP_EAFNOSUPPORT	47	/**< Address family not supported by protocol family */
#define	NFP_EADDRINUSE		48	/**< Address already in use */
#define	NFP_EADDRNOTAVAIL	49	/**< Can't assign requested address */

/* ipc/network software -- operational errors */
#define	NFP_ENETDOWN		50	/**< Network is down */
#define	NFP_ENETUNREACH		51	/**< Network is unreachable */
#define	NFP_ENETRESET		52	/**< Network dropped connection on reset */
#define	NFP_ECONNABORTED	53	/**< Software caused connection abort */
#define	NFP_ECONNRESET		54	/**< Connection reset by peer */
#define	NFP_ENOBUFS		55	/**< No buffer space available */
#define	NFP_EISCONN		56	/**< Socket is already connected */
#define	NFP_ENOTCONN		57	/**< Socket is not connected */
#define	NFP_ESHUTDOWN		58	/**< Can't send after socket shutdown */
#define	NFP_ETOOMANYREFS	59	/**< Too many references: can't splice */
#define	NFP_ETIMEDOUT		60	/**< Operation timed out */
#define	NFP_ECONNREFUSED	61	/**< Connection refused */
#define	NFP_ELOOP		62	/**< Too many levels of symbolic links */
#define	NFP_ENAMETOOLONG	63	/**< File name too long */
#define	NFP_EHOSTDOWN		64	/**< Host is down */
#define	NFP_EHOSTUNREACH	65	/**< No route to host */
#define	NFP_ENOTEMPTY		66	/**< Directory not empty */

/* quotas & mush */

#define	NFP_EPROCLIM		67	/**< Too many processes */
#define	NFP_EUSERS		68	/**< Too many users */
#define	NFP_EDQUOT		69	/**< Disc quota exceeded */

/* Network File System */
#define	NFP_ESTALE		70	/**< Stale NFS file handle */
#define	NFP_EREMOTE		71	/**< Too many levels of remote in path */
#define	NFP_EBADRPC		72	/**< RPC struct is bad */
#define	NFP_ERPCMISMATCH	73	/**< RPC version wrong */
#define	NFP_EPROGUNAVAIL	74	/**< RPC prog. not avail */
#define	NFP_EPROGMISMATCH	75	/**< Program version wrong */
#define	NFP_EPROCUNAVAIL	76	/**< Bad procedure for program */

#define	NFP_ENOLCK		77	/**< No locks available */
#define	NFP_ENOSYS		78	/**< Function not implemented */

#define	NFP_EFTYPE		79	/**< Inappropriate file type or format */
#define	NFP_EAUTH		80	/**< Authentication error */
#define	NFP_ENEEDAUTH		81	/**< Need authenticator */
#define	NFP_EIDRM		82	/**< Identifier removed */
#define	NFP_ENOMSG		83	/**< No message of desired type */
#define	NFP_EOVERFLOW		84	/**< Value too large to be stored in data type */
#define	NFP_ECANCELED		85	/**< Operation canceled */
#define	NFP_EILSEQ		86	/**< Illegal byte sequence */
#define	NFP_ENOATTR		87	/**< Attribute not found */

#define	NFP_EDOOFUS		88	/**< Programming error */

#define	NFP_EBADMSG		89	/**< Bad message */
#define	NFP_EMULTIHOP		90	/**< Multihop attempted */
#define	NFP_ENOLINK		91	/**< Link has been severed */
#define	NFP_EPROTO		92	/**< Protocol error */

#define	NFP_ENOTCAPABLE		93	/**< Capabilities insufficient */
#define	NFP_ECAPMODE		94	/**< Not permitted in capability mode */

#define	NFP_ELAST		94	/**< Must be equal largest errno */

/* pseudo-errors returned inside kernel to modify return to process */
#define	NFP_ERESTART	(-1)		/**< restart syscall */
#define	NFP_EJUSTRETURN	(-2)		/**< don't modify regs, just return */
#define	NFP_ENOIOCTL	(-3)		/**< ioctl not handled by this layer */
#define	NFP_EDIRIOCTL	(-4)		/**< do direct ioctl in GEOM */

extern __thread int nfp_errno;

const char *nfp_strerror(int errnum);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_ERRNO_H__ */
