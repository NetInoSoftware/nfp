/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */
#ifndef __NETWRAP_ERRNO_H__
#define __NETWRAP_ERRNO_H__

#include <errno.h>
#include "nfp_errno.h"

#define NETWRAP_ERRNO(x) ((x >= 0 && x <= NFP_ELAST) ?  netwrap_errno[x] : x)

static const int netwrap_errno[NFP_ELAST + 1] = {
0,
EPERM,			/* NFP_EPERM		1 */
ENOENT,		/* NFP_ENOENT		2 */
ESRCH,			/* NFP_ESRCH		3 */
EINTR,			/* NFP_EINTR		4 */
EIO,			/* NFP_EIO		5 */
ENXIO,			/* NFP_ENXIO		6 */
E2BIG,			/* NFP_E2BIG		7 */
ENOEXEC,		/* NFP_ENOEXEC		8 */
EBADF,			/* NFP_EBADF		9 */

ECHILD,		/* NFP_ECHILD		10 */
EDEADLK,		/* NFP_EDEADLK		11 */
ENOMEM,		/* NFP_ENOMEM		12 */
EACCES,		/* NFP_EACCES		13 */
EFAULT,		/* NFP_EFAULT		14 */
ENOTBLK,		/* NFP_ENOTBLK		15 */
EBUSY,			/* NFP_EBUSY		16 */
EEXIST,		/* NFP_EEXIST		17 */
EXDEV,			/* NFP_EXDEV		18 */
ENODEV,		/* NFP_ENODEV		19 */

ENOTDIR,		/* NFP_ENOTDIR		20 */
EISDIR,		/* NFP_EISDIR		21 */
EINVAL,		/* NFP_EINVAL		22 */
ENFILE,		/* NFP_ENFILE		23 */
EMFILE,		/* NFP_EMFILE		24 */
ENOTTY,		/* NFP_ENOTTY		25 */
ETXTBSY,		/* NFP_ETXTBSY		26 */
EFBIG,			/* NFP_EFBIG		27 */
ENOSPC,		/* NFP_ENOSPC		28 */
ESPIPE,		/* NFP_ESPIPE		29 */


EROFS,			/* NFP_EROFS		30 */
EMLINK,		/* NFP_EMLINK		31 */
EPIPE,			/* NFP_EPIPE		32 */
/* math software */
EDOM,			/* NFP_EDOM		33 */
ERANGE,		/* NFP_ERANGE		34 */
/* non-blocking and interrupt i/o */
EAGAIN,		/* NFP_EAGAIN		35 */

EINPROGRESS,		/* NFP_EINPROGRESS	36 */
EALREADY,		/* NFP_EALREADY		37 */

/* ipc/network software -- argument errors */
ENOTSOCK,		/* NFP_ENOTSOCK		38 */
EDESTADDRREQ,		/* NFP_EDESTADDRREQ	39 */

EMSGSIZE,		/* NFP_EMSGSIZE		40 */
EPROTOTYPE,		/* NFP_EPROTOTYPE	41 */
ENOPROTOOPT,		/* NFP_ENOPROTOOPT	42 */
EPROTONOSUPPORT,	/* NFP_EPROTONOSUPPORT	43 */
ESOCKTNOSUPPORT,	/* NFP_ESOCKTNOSUPPORT	44 */
EOPNOTSUPP,		/* NFP_EOPNOTSUPP	45 */
EPFNOSUPPORT,		/* NFP_EPFNOSUPPORT	46 */
EAFNOSUPPORT,		/* NFP_EAFNOSUPPORT	47 */
EADDRINUSE,		/* NFP_EADDRINUSE	48 */
EADDRNOTAVAIL,		/* NFP_EADDRNOTAVAIL	49 */


/* ipc/network software -- operational errors */
ENETDOWN,		/* NFP_ENETDOWN		50 */
ENETUNREACH,		/* NFP_ENETUNREACH	51 */
ENETRESET,		/* NFP_ENETRESET	52 */
ECONNABORTED,		/* NFP_ECONNABORTED	53 */
ECONNRESET,		/* NFP_ECONNRESET	54 */
ENOBUFS,		/* NFP_ENOBUFS		55 */
EISCONN,		/* NFP_EISCONN		56 */
ENOTCONN,		/* NFP_ENOTCONN		57 */
ESHUTDOWN,		/* NFP_ESHUTDOWN	58 */
ETOOMANYREFS,		/* NFP_ETOOMANYREFS	59 */


ETIMEDOUT,		/* NFP_ETIMEDOUT	60 */
ECONNREFUSED,		/* NFP_ECONNREFUSED	61 */
ELOOP,			/* NFP_ELOOP		62 */
ENAMETOOLONG,		/* NFP_ENAMETOOLONG	63 */
EHOSTDOWN,		/* NFP_EHOSTDOWN	64 */
EHOSTUNREACH,		/* EHOSTUNREACH		65 */
ENOTEMPTY,		/* NFP_ENOTEMPTY	66 */

/* quotas & mush */
EINVAL,		/* NFP_EPROCLIM		67 */
EUSERS,		/* NFP_EUSERS		68 */
EDQUOT,		/* NFP_EDQUOT		69 */


/* Network File System */
ESTALE,		/* NFP_ESTALE		70 */
EREMOTE,		/* NFP_EREMOTE		71 */
EINVAL,		/* NFP_EBADRPC		72 */
EINVAL,		/* NFP_ERPCMISMATCH	73 */
EINVAL,		/* NFP_EPROGUNAVAIL	74 */
EINVAL,		/* NFP_EPROGMISMATCH	75 */
EINVAL,		/* NFP_EPROCUNAVAIL	76 */
ENOLCK,		/* NFP_ENOLCK		77 */
ENOSYS,		/* NFP_ENOSYS		78 */
EINVAL,		/* NFP_EFTYPE		79 */


EINVAL,		/* NFP_EAUTH		80 */
EINVAL,		/* NFP_ENEEDAUTH	81 */
EIDRM,			/* NFP_EIDRM		82 */
ENOMSG,		/* NFP_ENOMSG		83 */
EOVERFLOW,		/* NFP_EOVERFLOW	84 */
ECANCELED,		/* NFP_ECANCELED	85 */
EILSEQ,		/* NFP_EILSEQ		86 */
EINVAL,		/* NFP_ENOATTR		87 */
EINVAL,		/* NFP_EDOOFUS		88 */
EBADMSG,		/* NFP_EBADMSG		89 */

EMULTIHOP,		/* NFP_EMULTIHOP	90 */
ENOLINK,		/* NFP_ENOLINK		91 */
EPROTO,		/* NFP_EPROTO		92 */
EINVAL,		/* NFP_ENOTCAPABLE	93 */
EINVAL			/* NFP_ECAPMODE		94 */
};

#endif /*__NETWRAP_ERRNO_H__*/