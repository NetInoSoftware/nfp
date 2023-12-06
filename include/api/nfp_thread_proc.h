/* Copyright (c) 2021 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_THREAD_PROC_H__
#define __NFP_THREAD_PROC_H__

#include <odp_api.h>
#include <odp/helper/odph_api.h>
#include <odp/helper/linux.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* NFP thread */
typedef struct {
	int (*start)(void *start_arg);  /**< Thread entry point function */
	void *arg;                      /**< Argument for the function */
	odp_thread_type_t thr_type;     /**< ODP thread type */
	const char *description;        /**< Thread description */
} nfp_thread_param_t;

typedef odph_thread_t nfp_thread_t;

/* NFP process */
typedef struct {
	odp_thread_type_t thr_type; /**< ODP thread type */
	const char *description;    /**< Process description */
} nfp_process_param_t;

typedef odph_linux_process_t nfp_process_t;

/**
 * Get default workers to cores distribution
 *
 * By default, first two cores are reserved for control
 * and linux processing while the rest are available for
 * workers. One run-to-completion thread or process can
 * be created for each worker core.
 *
 * Application may use custom distributions. The right
 * distribution depends on usecase and number of cores
 * available.
 *
 * @param req_num The number of worker cores requested. If set 0 then
 * the maximum number of available cores is requested (see 'req_num_max').
 * @param req_num_max If not zero, it represents the upper limit for
 * the number of worker cores returned in 'cpumask'. This parameter is
 * especially useful when 'req_num' is set to zero.
 * @param cpumask The worker to core distribution.
 * @retval 0 Success.
 * @retval -1 Failure.
 */

int nfp_get_default_worker_cpumask(int req_num, int req_num_max,
				   odp_cpumask_t *cpumask);

/**
 * Initialize a nfp_thread_param_t argument with the default values.
 *
 * @param param    Argument to initialize.
 **/

void nfp_thread_param_init(nfp_thread_param_t *param);

/**
 * Creates and launches nfp threads
 *
 * Creates, pins and launches threads to separate CPU's based on the cpumask.
 *
 * @param thread_tbl    Thread table
 * @param num           Number of threads to start
 * @param mask          CPU mask
 * @param thr_params    NFP thread parameters
 *
 * @return Number of threads created
 */
int nfp_thread_create(nfp_thread_t *thread_tbl,
		      int num,
		      const odp_cpumask_t *cpumask,
		      const nfp_thread_param_t *thr_param);

/**
 * Waits nfp threads to exit.
 *
 * Returns when all threads have terminated.
 *
 * @param thread_tbl    Table of threads to exit
 * @param num           Number of threads to exit
 * @return The number of joined threads or -1 on error.
 * (error occurs if any of the start_routine return non-zero or if
 *  the thread join/process wait itself failed -e.g. as the result of a kill)
 */

int nfp_thread_join(nfp_thread_t *thread_tbl, int num);

/**
 * Initialize a nfp_process_param_t argument with the default values.
 *
 * @param param    Argument to initialize.
 **/

void nfp_process_param_init(nfp_process_param_t *param);

/**
 * Fork a number of nfp processes
 *
 * Forks and sets CPU affinity for child processes.
 *
 * @param[out] proc_tbl    Process state info table (for output)
 * @param      mask        CPU mask of processes to create
 * @param      thr_params  NFP process parameters
 *
 * @return On success: 1 for the parent, 0 for the child
 *         On failure: -1 for the parent, -2 for the child
 */
int nfp_process_fork_n(nfp_process_t *proc_tbl,
		       const odp_cpumask_t *mask,
		       const nfp_process_param_t *proc_param);

/**
 * Wait for a number of nfp processes to end
 *
 * Waits for a number of child processes to terminate.
 *
 * @param proc_tbl      Process state info table (previously filled by fork)
 * @param num           Number of processes to wait
 *
 * @return 0 on success, -1 on failure
 */
int nfp_process_wait_n(nfp_process_t *proc_tbl, int num);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_THREAD_PROC_H__ */

