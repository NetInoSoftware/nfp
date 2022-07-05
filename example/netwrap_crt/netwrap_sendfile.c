/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <sys/types.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <odp_api.h>
#include "nfp.h"
#include "netwrap_sendfile.h"
#include "netwrap_errno.h"


static ssize_t (*libc_sendfile64)(int, int, off_t *, size_t);
static ssize_t (*libc_read)(int, void*, size_t);

void setup_sendfile_wrappers(void)
{
	LIBC_FUNCTION(sendfile64);
	LIBC_FUNCTION(read);
}

#define BUF_SIZE 1024

ssize_t sendfile64(int out_fd, int in_fd, off64_t *offset, size_t count)
{
	ssize_t sendfile_value = -1;

	if (IS_NFP_SOCKET(out_fd)) {
		off_t orig = 0;
		size_t data_processed = 0;
		char buff[BUF_SIZE];
		ssize_t data_read;
		nfp_ssize_t nfp_data_sent;
		nfp_ssize_t nfp_data_sent_sum;

		if (offset != NULL) {
			orig = lseek(in_fd, 0, SEEK_CUR);
			if (orig == (off_t)-1)
				return -1;
			if (lseek(in_fd, *offset, SEEK_SET) == -1)
				return -1;
		}

		while (data_processed < count) {
			data_read = (*libc_read)(in_fd, buff, BUF_SIZE);
			if (data_read < 0)
				return -1;
			else if (data_read == 0)   /*EOF*/
				break;

			nfp_data_sent_sum = 0;
			while (nfp_data_sent_sum < data_read) {
				nfp_data_sent = nfp_send(out_fd,
					buff + nfp_data_sent_sum,
					data_read - nfp_data_sent_sum, 0);
				if (nfp_data_sent < 0) {
					if (nfp_errno == NFP_EWOULDBLOCK) {
						usleep(100);
						continue;
					}
					errno = NETWRAP_ERRNO(nfp_errno);
					return -1;
				}
				nfp_data_sent_sum += nfp_data_sent;
			}
			data_processed += data_read;
		}

		sendfile_value = data_processed;

		if (offset != NULL) {
			*offset = lseek(in_fd, 0, SEEK_CUR);
			if (*offset == -1)
				return -1;
			if (lseek(in_fd, orig, SEEK_SET) == -1)
				return -1;
		}
	} else if (libc_sendfile64)
		sendfile_value = (*libc_sendfile64)(out_fd, in_fd,
				offset, count);
	else {
		LIBC_FUNCTION(sendfile64);

		if (libc_sendfile64)
			sendfile_value = (*libc_sendfile64)(out_fd, in_fd,
				offset, count);
		else {
			sendfile_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Sendfile64 called on socket '%d' returned:'%d'\n",
		out_fd, (int)sendfile_value);*/
	return sendfile_value;
}
