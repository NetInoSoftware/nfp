/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <odp_api.h>
#include "nfp.h"
#include "netwrap_ioctl.h"
#include "netwrap_errno.h"

static int (*libc_ioctl)(int, unsigned long int, ...);

void setup_ioctl_wrappers(void)
{
	LIBC_FUNCTION(ioctl);
}

int ioctl(int fd, unsigned long int request, ...)
{
	int ioctl_value;
	va_list ap;
	void *p;

	va_start(ap, request);
	p = va_arg(ap, void *);
	va_end(ap);

	if (IS_NFP_SOCKET(fd)) {
		int nfp_request;

		if (request == FIONREAD)
			nfp_request = NFP_FIONREAD;
		else if (request == FIONBIO)
			nfp_request = NFP_FIONBIO;
		else if (request == FIOASYNC)
			nfp_request = NFP_FIOASYNC;
		/*else if (request == FIONWRITE)
			nfp_request = NFP_FIONWRITE;
		else if (request == FIONSPACE)
			nfp_request = NFP_FIONSPACE;*/
		else {
			errno = EINVAL;
			return -1;
		}
		ioctl_value = nfp_ioctl(fd, nfp_request, p);
		errno = NETWRAP_ERRNO(nfp_errno);
	} else if (libc_ioctl)
		ioctl_value = (*libc_ioctl)(fd, request, p);
	else {
		LIBC_FUNCTION(ioctl);
		if (libc_ioctl)
			ioctl_value = (*libc_ioctl)(fd, request, p);
		else {
			ioctl_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Ioctl called on socket '%d' returned %d\n", fd,
		ioctl_value);*/
	return ioctl_value;
}
