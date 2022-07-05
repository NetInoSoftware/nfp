/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "socket_shutdown.h"
#include "socket_util.h"


int shutdown_socket(int fd)
{
	if (nfp_shutdown(fd, NFP_SHUT_RDWR) == -1) {
		NFP_ERR("Failed to shutdown socket (errno = %d)\n",
			nfp_errno);
		return -1;
	}

	NFP_INFO("SUCCESS.\n");
	return 0;
}
