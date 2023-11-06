/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_EXAMPLE_SIGACTION__
#define __NFP_EXAMPLE_SIGACTION__

int nfpexpl_sigaction_set(void (*sig_func)(int));

void nfpexpl_sigfunction_stop(int signum);

#endif /* __NFP_EXAMPLE_SIGACTION__ */
