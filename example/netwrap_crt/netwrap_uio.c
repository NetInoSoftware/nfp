/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <sys/uio.h>
#include <unistd.h>
#include <odp_api.h>
#include "nfp.h"
#include "netwrap_uio.h"
#include "netwrap_errno.h"

static ssize_t (*libc_writev)(int, const struct iovec *, int);

void setup_uio_wrappers(void)
{
	LIBC_FUNCTION(writev);
}


ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
	ssize_t writev_value = -1;

	if (IS_NFP_SOCKET(fd)) {
		int i;
		ssize_t writev_sum = 0;
		nfp_size_t iov_len;
		nfp_size_t iov_snt;
		char *iov_base;
		nfp_ssize_t nfp_send_res;


		for (i = 0; i < iovcnt; i++) {
			iov_len = iov[i].iov_len;
			iov_snt = 0;
			iov_base = (char *)iov[i].iov_base;

			while (iov_snt < iov_len) {
				nfp_send_res = nfp_send(fd, iov_base + iov_snt,
					iov_len - iov_snt, 0);

				if (nfp_send_res <= 0) {
					if (nfp_send_res == 0 ||
						nfp_errno == NFP_EAGAIN) {
						usleep(100);
						continue;
					}
					errno = NETWRAP_ERRNO(nfp_errno);
					return -1;
				}
				iov_snt += nfp_send_res;
			}
			writev_sum += iov_len;
		}
		writev_value = writev_sum;
	} else if (libc_writev)
		writev_value = (*libc_writev)(fd, iov, iovcnt);
	else {
		LIBC_FUNCTION(writev);

		if (libc_writev)
			writev_value = (*libc_writev)(fd, iov, iovcnt);
		else {
			writev_value = -1;
			errno = EACCES;
		}
	}

	return writev_value;
}
