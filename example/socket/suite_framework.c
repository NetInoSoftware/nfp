/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "nfp.h"
#include "suite_framework.h"

static int suite_thread1(void *arg);
static int suite_thread2(void *arg);

int fd_thread1 = -1;
int fd_thread2 = -1;
int core_id = -1;

int config_suite_framework(uint16_t linux_core_id)
{
	core_id = linux_core_id;

	return 0;
}

int init_suite(init_function init_func)
{
	fd_thread1 = -1;
	fd_thread2 = -1;

	if (init_func)
		return init_func(&fd_thread1, &fd_thread2);
	else
		return 0;
}

void run_suite(run_function run_func1, run_function run_func2)
{
	nfp_thread_t sock_pthread1;
	nfp_thread_t sock_pthread2;
	odp_cpumask_t sock_cpumask;
	nfp_thread_param_t thread_param;

	odp_cpumask_zero(&sock_cpumask);
	odp_cpumask_set(&sock_cpumask, core_id);

	nfp_thread_param_init(&thread_param);
	thread_param.start = suite_thread1;
	thread_param.arg = run_func1;
	thread_param.thr_type = ODP_THREAD_CONTROL;
	nfp_thread_create(&sock_pthread1, 1,
			  &sock_cpumask, &thread_param);

	nfp_thread_param_init(&thread_param);
	thread_param.start = suite_thread2;
	thread_param.arg = run_func2;
	thread_param.thr_type = ODP_THREAD_CONTROL;
	nfp_thread_create(&sock_pthread2, 1,
			  &sock_cpumask, &thread_param);

	nfp_thread_join(&sock_pthread1, 1);
	nfp_thread_join(&sock_pthread2, 1);
}

void end_suite(void)
{
	if (fd_thread1 != -1) {
		if (nfp_close(fd_thread1) == -1)
			NFP_ERR("Failed to close socket 1 (errno = %d)\n",
				nfp_errno);
		fd_thread1 = -1;
	}

	if (fd_thread2 != -1) {
		if (nfp_close(fd_thread2) == -1)
			NFP_ERR("Failed to close socket 1 (errno = %d)\n",
				nfp_errno);
		fd_thread2 = -1;
	}
}

static int suite_thread1(void *arg)
{
	run_function run_func = (run_function)arg;

	(void)run_func(fd_thread1);

	return 0;
}

static int suite_thread2(void *arg)
{
	run_function run_func = (run_function)arg;

	(void)run_func(fd_thread2);

	return 0;
}
