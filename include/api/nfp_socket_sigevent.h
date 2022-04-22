/* Copyright (c) 2021 Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#ifndef __NFP_SOCKET_SIGEVENT_H__
#define __NFP_SOCKET_SIGEVENT_H__

#include <odp_api.h>
#include "nfp_socket.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/* Sigevent event type*/
#define NFP_EVENT_INVALID	0
#define NFP_EVENT_ACCEPT	1
#define NFP_EVENT_RECV	2
#define NFP_EVENT_SEND	3

union nfp_sigval {          /* Data passed with notification */
	int     sival_int;         /* Integer value */
	void   *sival_ptr;         /* Pointer value */
};

/**
 * struct nfp_sock_sigval is the argument of notification function
 *
 * sigev_value: "nfp_sigevent.sigev_value" argument configured with
 * nfp_socket_sigevent().
 *
 * nfp_sock_sigval.event:  indicates the type of event received. Valid values
 * are NFP_EVENT_ACCEPT, NFP_EVENT_RECV or NFP_EVENT_SEND.
 *
 * sockfd: socket on which event occurred
 *
 * sockfd2: additional socket e.g. (accepted socket on NFP_EVENT_ACCEPT event)
 *
 * pkt: the packet triggering the event (NFP_EVENT_RECV only)
 *
 * Note: On NFP_EVENT_RECV events, the packet is considered
 * processed 'outside the stack' if the 'pkt' is set to ODP_PACKET_INVALID
 * at the return of notification function. One may inspect, process the
 * packet in different ways or even delete it in the notification function.
 */

struct nfp_sock_sigval {
	union nfp_sigval    sigev_value; /* Data passed with notification
					from event configuration api
					(struct nfp_sigevent)*/
	int                 event;	/* Sigevent event type */
	int                 sockfd;	/* socket on which event occurred*/
	int                 sockfd2;	/* additional socket e.g. (accepted
					socket on NFP_EVENT_ACCEPT event */

	odp_packet_t        pkt;	/* The packet triggering the event */
};

/* Sigevent notification method */
#define NFP_SIGEV_NONE 0
#define NFP_SIGEV_HOOK 1
#define NFP_SIGEV_SIGNAL 2
#define NFP_SIGEV_THREAD 3

struct nfp_sigevent {
	int                 sigev_notify; /* Notification method */
	int                 sigev_signo;  /* Notification signal */
	union nfp_sigval    sigev_value;  /* Data passed with notification */
	void                (*sigev_notify_func)(union nfp_sigval *sigev_value);
		/* Function used for notification */
	void                *sigev_notify_attr;
		/* Attributes for notification thread
		(SIGEV_THREAD) */
	nfp_pid_t           sigev_notify_thread_id;
		/* ID of thread to signal (SIGEV_THREAD_ID) */
} ODP_ALIGNED_CACHE;

/**
 * Configures the event notification on a socket.
 *
 * Notification mechanism is configured through a "struct nfp_sigevent"
 * argument:
 * nfp_sigevent.sigev_notify specifies the type of notification that is
 * requested. In current version, only NFP_SIGEV_HOOK type is supported.
 *
 * nfp_sigevent.sigev_notify_func is the callback function.
 * The argument of this function has a "union nfp_sigval *" type but
 * the actual type depends on nfp_sigevent.sigev_notify field.
 * In current version, a "struct nfp_sock_sigval *" is returned.
 *
 * In the future, the function's argument may differ depending on
 * "nfp_sock_sigval.event" type (base type + derived type model).
 *
 * nfp_sigevent.sigev_value is a field that will be passed to
 * the callback function as (part of the) function's argument
 * (e.g. nfp_sock_sigval.sigev_value).
 *
 * The rest of "struct nfp_sigevent" fields are not used.
 *
 * Note: On TCP, the event notification configuration will be inherited from
 * listening socket to accepted socket.
 *
 * @param sd            Socket descriptor
 * @param ev            Event notification parameters
 *
 * @return 0 on success
 * @retval !0 on error
 */
int	nfp_socket_sigevent(int sd, struct nfp_sigevent *ev);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_SOCKET_SIGEVENT_H__ */

