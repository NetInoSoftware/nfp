/* Copyright (c) 2016, ENEA Software AB
 * Copyright (c) 2016, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <odp_api.h>
#include "nfp.h"
#include "netwrap_select.h"
#include "netwrap_errno.h"

static int (*libc_select)(int, fd_set *, fd_set *, fd_set *,
	struct timeval *);


void setup_select_wrappers(void)
{
	LIBC_FUNCTION(select);
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
	fd_set *exceptfds, struct timeval *timeout)
{
	int select_value;

	if (IS_NFP_SOCKET((nfds - 1))) {
		nfp_fd_set nfp_readfds, nfp_readfds_bku;
		struct nfp_timeval nfp_timeout_local;
		struct nfp_timeval *nfp_timeout;
		int i;
		uint32_t period_usec = 0;
		uint32_t temp_period_usec = 0;

		(void)writefds;
		(void)exceptfds;

		if (!readfds) {
			errno = EBADF;
			return -1;
		}

		NFP_FD_ZERO(&nfp_readfds_bku);
		for (i = nfp_global_params.socket.sd_offset; i < nfds; i++)
			if (FD_ISSET(i, readfds))
				NFP_FD_SET(i, &nfp_readfds_bku);

		nfp_timeout = &nfp_timeout_local;
		nfp_timeout_local.tv_sec = 0;
		nfp_timeout_local.tv_usec = 0;

		if (timeout)
			period_usec = timeout->tv_sec * 1000000UL +
				timeout->tv_usec;

		do {
			memcpy(&nfp_readfds, &nfp_readfds_bku,
					sizeof(nfp_readfds));
			select_value = nfp_select(nfds, &nfp_readfds, NULL,
				NULL, nfp_timeout);
			if (select_value)
				break;
			else if (!timeout)
				continue;
			else {
				usleep(100);
				temp_period_usec += 100;
				if (temp_period_usec > period_usec) {
					select_value = 0;
					break;
				}
			}

		} while (1);
		errno = NETWRAP_ERRNO(nfp_errno);

		if (select_value > 0) {
			for (i = nfp_global_params.socket.sd_offset;
			     i < nfds; i++)
				if (FD_ISSET(i, readfds) &&
					!NFP_FD_ISSET(i, &nfp_readfds))
						FD_CLR(i, readfds);
		} else if (select_value == 0)
			FD_ZERO(readfds);

		if (!nfp_errno && timeout) {
			timeout->tv_sec = nfp_timeout_local.tv_sec;
			timeout->tv_usec = nfp_timeout_local.tv_usec;
		}
	} else if (libc_select)
		select_value = (*libc_select)(nfds, readfds, writefds,
			exceptfds, timeout);
	else {
		LIBC_FUNCTION(select);

		if (libc_select)
			select_value = (*libc_select)(nfds, readfds, writefds,
				exceptfds, timeout);
		else {
			select_value = -1;
			errno = EACCES;
		}
	}

	/*printf("Select called with max fd = %d returned %d\n",
		nfds, select_value);*/
	return select_value;
}
