/* Copyright (c) 2014, Linaro Limited
 * All rights reserved.
 * Copyright (c) 2014, Nokia
 * Copyright (c) 2014, Enea Software AB
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */
/**
 * @file
 *
 * nfp log
 */

#ifndef __NFP_LOG_H__
#define __NFP_LOG_H__

#include <odp_api.h>
#include "nfp_timer.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/*
 * These logging macros can be used to send a message to the logging
 * destination. Currently, this is stderr.
 *
 * Log line format:
 *
 *   L t coreid:threadid file:line] msg...
 *
 * where the fields are defined as:
 *
 *   L        - A single character, representing the log level
 *   t        - ODP tick count
 *   coreid   - ODP core id
 *   threadid - pthread thread id
 *   file     - The file name
 *   line     - The line number
 *   msg      - The user-supplied message
 *
 * Example:
 *
 *   D 186 3:3098175232 nfp_pkt_processing.c:280] Device IP: 0.0.0.0, Packet Dest IP: 192.168.146.130
 */
#define NFP_INFO(fmt, ...) \
	_NFP_LOG(NFP_LOG_INFO, fmt, ##__VA_ARGS__)
#define NFP_WARN(fmt, ...) \
	_NFP_LOG(NFP_LOG_WARNING, fmt, ##__VA_ARGS__)
#define NFP_ERR(fmt, ...) \
	_NFP_LOG(NFP_LOG_ERROR, fmt, ##__VA_ARGS__)

/*
 * Debug macros which will be compiled out when --enable-debug is not used.
 */
#if defined(NFP_DEBUG)
#define NFP_DBG(fmt, ...) \
	_NFP_LOG(NFP_LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define NFP_DBG(fmt, ...) do {} while (0)
#endif

#define NFP_LOGLEVEL_GET nfp_loglevel_get()

#define NFP_LOG_NO_CTX(level, fmt, ...) do {		\
		if (level > NFP_LOGLEVEL_GET)		\
			break;				\
		fprintf(stderr, fmt, ##__VA_ARGS__);	\
	} while (0)

#define NFP_LOG_NO_CTX_NO_LEVEL(fmt, ...) \
		fprintf(stderr, fmt, ##__VA_ARGS__)

enum nfp_log_level_s {
	NFP_LOG_DISABLED = 0,
	NFP_LOG_ERROR,
	NFP_LOG_WARNING,
	NFP_LOG_INFO,
	NFP_LOG_DEBUG,
	NFP_LOG_MAX_LEVEL
};

/**
 * Get log level
 *
 * @retval log level
*/
enum nfp_log_level_s nfp_loglevel_get(void);

/**
 * Set log level
 *
 * @param loglevel log level to be set
*/
void nfp_loglevel_set(enum nfp_log_level_s loglevel);

/**
 * Check if logging level is 'debug'
 *
 * @retval !0 debug log level is set
 * @retval 0 debug log level is not set
 *
*/
int nfp_debug_logging_enabled(void);

/*
 * Do not use these macros.
 */
#define __FILENAME__ \
	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define _NFP_LOG(level, fmt, ...) do {					\
		if (level > NFP_LOGLEVEL_GET)				\
			break;						\
		fprintf(stderr, "%s %d %d:%u %s:%d] " fmt "\n",		\
			(level == NFP_LOG_ERROR)   ? "E" :		\
			(level == NFP_LOG_WARNING) ? "W" :		\
			(level == NFP_LOG_INFO)    ? "I" :		\
			(level == NFP_LOG_DEBUG)   ? "D" : "?",		\
			nfp_timer_ticks(0),				\
			odp_cpu_id(), (unsigned int) pthread_self(),	\
			__FILENAME__, __LINE__,				\
			##__VA_ARGS__);					\
	} while (0)

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /*__NFP_LOG_H__*/
