/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "nfp.h"
#include "socket_util.h"
#include "suite_framework.h"
#include "socket_create_close.h"
#include "socket_bind.h"
#include "socket_shutdown.h"
#include "socket_connect_udp.h"
#include "socket_send_sendto_udp.h"
#include "socket_send_recv_udp.h"
#include "socket_listen_tcp.h"
#include "socket_connect_accept_tcp.h"
#include "socket_send_recv_tcp.h"
#include "socket_select.h"
#include "socket_sigevent.h"
#include "socket_sendmsg_recvmsg.h"
#include "socket_getsockname.h"
#include "socket_getpeername.h"
#include "socket_getaddrinfo.h"
#include "socket_getnameinfo.h"
#include "socket_getservent.h"

#define MAX_WORKERS		32

/** Get rid of path in filename - only for unix-type paths using '/' */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

/**
 * Parsed command line application arguments
 */
typedef struct {
	int core_count;
	int if_count;		/**< Number of interfaces to be used */
	char **if_names;	/**< Array of pointers to interface names */
	char *cli_file;
	odp_bool_t single_pkt_API;
} appl_args_t;

/* helper funcs */
static int configure_interface_addresses(void);
static int print_interfaces(void);
static void parse_args(int argc, char *argv[], appl_args_t *appl_args);
static void print_info(char *progname, appl_args_t *appl_args,
		       odp_cpumask_t *cpumask);
static void usage(char *progname);

static int null_function(int fd)
{
	(void)fd;
	return 0;
}

/** main() Application entry point
 *
 * @param argc int
 * @param argv[] char*
 * @return int
 *
 */
int main(int argc, char *argv[])
{
	appl_args_t params;
	nfp_initialize_param_t app_init_params;
	nfp_thread_t thread_tbl[MAX_WORKERS];
	nfp_thread_param_t thread_param;
	int num_workers, ret_val, i;
	odp_cpumask_t cpumask_workers;

	/* Parse and store the application arguments */
	parse_args(argc, argv, &params);

	/*
	 * This example assumes that core #0 and #1 runs Linux kernel
	 * background tasks and control threads.
	 * By default, cores #2 and beyond will be populated with a NFP
	 * processing threads each.
	 */
	nfp_initialize_param(&app_init_params);
	app_init_params.cli.os_thread.start_on_init = 1;
	app_init_params.if_count = params.if_count;
	for (i = 0; i < params.if_count && i < NFP_FP_INTERFACE_MAX; i++) {
		strncpy(app_init_params.if_names[i], params.if_names[i],
			NFP_IFNAMSIZ);
		app_init_params.if_names[i][NFP_IFNAMSIZ - 1] = '\0';
	}

	if (nfp_initialize(&app_init_params)) {
		NFP_ERR("Error: NFP global init failed.\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * Get the default workers to cores distribution: one
	 * run-to-completion worker thread or process can be created per core.
	 */
	if (nfp_get_default_worker_cpumask(params.core_count, MAX_WORKERS,
					   &cpumask_workers)) {
		NFP_ERR("Error: Failed to get the default workers to cores "
			"distribution\n");
		nfp_terminate();
		return EXIT_FAILURE;
	}
	num_workers = odp_cpumask_count(&cpumask_workers);

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params, &cpumask_workers);

	/* Start dataplane dispatcher worker threads */
	memset(thread_tbl, 0, sizeof(thread_tbl));
	nfp_thread_param_init(&thread_param);
	if (params.single_pkt_API) {
		thread_param.start = default_event_dispatcher;
		thread_param.arg = nfp_eth_vlan_processing;
	} else {
		thread_param.start = default_event_dispatcher_multi;
		thread_param.arg = nfp_eth_vlan_processing_multi;
	}
	thread_param.thr_type = ODP_THREAD_WORKER;

	ret_val = nfp_thread_create(thread_tbl, num_workers,
				    &cpumask_workers, &thread_param);
	if (ret_val != num_workers) {
		NFP_ERR("Error: Failed to create worker threads, "
			"expected %d, got %d",
			num_workers, ret_val);
		nfp_stop_processing();
		if (ret_val != -1)
			nfp_thread_join(thread_tbl, ret_val);
		nfp_terminate();
		return EXIT_FAILURE;
	}

	sleep(2);

	if (params.cli_file) {
		/* Configure IP addresses from CLI file */
		nfp_cli_process_file(params.cli_file);
	} else {
		/* Configure default IP addresses*/
		ret_val = configure_interface_addresses();
		if (ret_val != 0) {
			nfp_stop_processing();
			nfp_thread_join(thread_tbl, num_workers);
			nfp_terminate();
			return EXIT_FAILURE;
		}
	}

	nfp_loglevel_set(NFP_LOG_INFO);

	sleep(5);

	print_interfaces();

	config_suite_framework(app_init_params.linux_core_id);

	NFP_INFO("\n\nSuite: IPv4 UDP socket: create and close.\n\n");
	if (!init_suite(NULL))
		run_suite(create_close_udp, create_close_udp_noproto);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket: create and close.\n\n");
	if (!init_suite(NULL))
		run_suite(create_close_tcp, create_close_tcp_noproto);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP socket: create and close.\n\n");
	if (!init_suite(NULL))
		run_suite(create_close_udp6, create_close_udp6_noproto);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket: create and close.\n\n");
	if (!init_suite(NULL))
		run_suite(create_close_tcp6, create_close_tcp6_noproto);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP socket: bind.\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(bind_ip4_local_ip, bind_ip4_any);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket: bind.\n\n");
	if (!init_suite(init_tcp_create_socket))
		run_suite(bind_ip4_local_ip, bind_ip4_any);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP socket: bind.\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(bind_ip6_local_ip, bind_ip6_any);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket: bind.\n\n");
	if (!init_suite(init_tcp6_create_socket))
		run_suite(bind_ip6_local_ip, bind_ip6_any);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP socket: shutdown.\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(shutdown_socket, shutdown_socket);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket: shutdown (no connection).\n\n");
	if (!init_suite(init_tcp_create_socket))
		run_suite(shutdown_socket, shutdown_socket);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP socket: shutdown.\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(shutdown_socket, shutdown_socket);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket: shutdown (no connection).\n\n");
	if (!init_suite(init_tcp6_create_socket))
		run_suite(shutdown_socket, shutdown_socket);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP socket: connect.\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(connect_udp4, connect_bind_udp4);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP socket: connect + shutdown.\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(connect_shutdown_udp4, connect_shutdown_bind_udp4);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP socket: connect.\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(connect_udp6, connect_bind_udp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP socket: connect + shutdown.\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(connect_shutdown_udp6, connect_shutdown_bind_udp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP socket: connect + shutdown + any.\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(connect_shutdown_udp6_any,
			  connect_shutdown_bind_udp6_any);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: send + sendto\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_ip4_udp_local_ip, sendto_ip4_udp_local_ip);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP socket bind any address: send + sendto\n\n");
	if (!init_suite(init_udp_bind_any))
		run_suite(send_ip4_udp_any, sendto_ip4_udp_any);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP socket BIND local address: send + sendto\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_ip6_udp_local_ip, sendto_ip6_udp_local_ip);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP socket bind any address: send + sendto\n\n");
	if (!init_suite(init_udp6_bind_any))
		run_suite(send_ip6_udp_any, sendto_ip6_udp_any);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP bind local IP: sendto + recv.\n\n");
	if (!init_suite(init_udp_local_ip))
		run_suite(send_udp_local_ip, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP bind loopback IP: sendto + recv.\n\n");
	if (!init_suite(init_udp_loopback))
		run_suite(send_udp_loopback, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP bind local IP: sendto + recvfrom.\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, recvfrom_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP bind any address: sendto + recv.\n\n");
	if (!init_suite(init_udp_any))
		run_suite(send_udp_any, recv_udp);
	end_suite();

	NFP_INFO("\n\nSuite: IPv4 UDP bind any address: sendto + recvfrom.\n\n");
	if (!init_suite(init_udp_bind_any))
		run_suite(send_udp_any, recvfrom_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP bind any address: sendto + recvfrom(NULL addr).\n\n");
	if (!init_suite(init_udp_bind_any))
		run_suite(send_udp_any, recvfrom_udp_null_addr);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: sendto + recv.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_udp6_local_ip, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: sendto + recvfrom.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_udp6_local_ip, recvfrom_udp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind any IP: sendto + recv.\n\n");
	if (!init_suite(init_udp6_bind_any))
		run_suite(send_udp6_any, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind any IP: sendto + recvfrom.\n\n");
	if (!init_suite(init_udp6_bind_any))
		run_suite(send_udp6_any, recvfrom_udp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind any IP: sendto + recvfrom(NULL addr).\n\n");
	if (!init_suite(init_udp6_bind_any))
		run_suite(send_udp6_any, recvfrom_udp_null_addr);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind loopback IP: sendto + recv.\n\n");
	if (!init_suite(init_udp6_loopback))
		run_suite(send_udp6_loopback, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 TCP socket local IP: listen.\n\n");
	if (!init_suite(init_tcp_bind_local_ip))
		run_suite(listen_tcp, listen_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP socket local IP: listen.\n\n");
	if (!init_suite(init_tcp6_bind_local_ip))
		run_suite(listen_tcp, listen_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 TCP socket local IP: connect + accept.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_tcp4_local_ip, accept_tcp4);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket any IP: connect + accept.\n\n");
	if (!init_suite(init_tcp_bind_listen_any))
		run_suite(connect_tcp4_any, accept_tcp4);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket local IP: connect + accept null address.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_tcp4_local_ip, accept_tcp4_null_addr);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP socket local IP: connect + accept.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(connect_tcp6_local_ip, accept_tcp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket any IP: connect + accept.\n\n");
	if (!init_suite(init_tcp6_bind_listen_any))
		run_suite(connect_tcp6_any, accept_tcp6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket local IP: connect + accept null address.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(connect_tcp6_local_ip, accept_tcp6_null_addr);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 TCP socket local IP: send + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, receive_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket any IP: send + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_any))
		run_suite(send_tcp4_any, receive_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket any IP: multi send + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_any))
		run_suite(send_multi_tcp4_any, receive_multi_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP socket any IP: 2 * send "
			"+ recv(NFP_MSG_WAITALL).\n\n");
	if (!init_suite(init_tcp_bind_listen_any))
		run_suite(send_tcp4_msg_waitall, receive_tcp4_msg_waitall);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP socket local IP: send + recv.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(send_tcp6_local_ip, receive_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP socket any IP: send + recv.\n\n");
	if (!init_suite(init_tcp6_bind_listen_any))
		run_suite(send_tcp6_any, receive_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 UDP bind local IP: select + recv.\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, select_recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: select + accept + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, select_recv_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: select + recv.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_udp6_local_ip, select_recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP bind local IP: select + accept + recv.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(send_tcp6_local_ip, select_recv_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 UDP bindlocal IP: select + recv x2.\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, select_recv_udp_2);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP bind local IP: socket_sigevent rcv.\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(recv_send_udp_local_ip, socket_sigevent_udp4);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: socket_sigevent rcv.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(recv_send_udp6_local_ip, socket_sigevent_udp6);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: socket_sigevent rcv.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_recv_send_tcp_local_ip,
			  socket_sigevent_tcp_rcv);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP bind local IP: socket_sigevent rcv.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(connect_recv_send_tcp6_local_ip,
			  socket_sigevent_tcp_rcv);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: socket_sigevent accept.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_tcp_delayed_local_ip,
			  socket_sigevent_tcp_accept);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP bind local IP: socket_sigevent accept.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(connect_tcp6_delayed_local_ip,
			  socket_sigevent_tcp_accept);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: "
		 "sendmsg + recv\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(sendmsg_ip4_udp_local_ip, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: "
		 "sendmsg (multi) + recv\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(sendmsg_multi_ip4_udp_local_ip, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: sendmsg + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_sendmsg_tcp_local_ip,
			  accept_receive);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: "
		 "sendmsg (multi) + recv.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(connect_sendmsg_multi_tcp_local_ip,
			  accept_receive);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: "
		 "sendmsg (multi) + recv.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(sendmsg_multi_ip6_udp_local_ip, recv_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP bind local IP: "
		 "sendmsg (multi) + recv.\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(connect_sendmsg_multi_ipv6_tcp_local_ip,
			  accept_receive);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: "
		 "sendto + recvmsg\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, recvmsg_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: "
		 "sendto + recvmsg (multi)\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, recvmsg_multi_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP socket BIND local address: "
		 "sendto + recvmsg (multi + peek)\n\n");
	if (!init_suite(init_udp_bind_local_ip))
		run_suite(send_udp_local_ip, recvmsg_multi_peek_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: sendto + recvmsg.\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_udp6_local_ip, recvmsg_udp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 UDP bind local IP: "
		 "sendto + recvmsg (multi).\n\n");
	if (!init_suite(init_udp6_bind_local_ip))
		run_suite(send_udp6_local_ip, recvmsg_multi_udp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: send + recvmsg.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, accept_recvmsg_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: "
		 "send + recvmsg (multi).\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, accept_recvmsg_multi_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 TCP bind local IP: "
		 "send + recvmsg (multi).\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(send_tcp6_local_ip, accept_recvmsg_multi_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: "
		 "send + recvmsg (multi x 2).\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, accept_recvmsg_multi2_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: "
		 "send + recvmsg (multi + peek).\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_local_ip, accept_recvmsg_multi_peek_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: sendx2 + recvmsg.\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_2_local_ip, accept_recvmsg_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP bind local IP: "
		 "sendx2 + recvmsg (multi + peek).\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(send_tcp4_2_local_ip, accept_recvmsg_multi_peek_tcp);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 UDP: getsockname + getsockname (bound).\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(getsockname_not_bind, getsockname_bind);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP: getsockname + getsockname (bound).\n\n");
	if (!init_suite(init_tcp_create_socket))
		run_suite(getsockname_not_bind, getsockname_bind);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP: getsockname + getsockname (bound).\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(getsockname_not_bind6, getsockname_bind6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP: getsockname + getsockname (bound).\n\n");
	if (!init_suite(init_tcp6_create_socket))
		run_suite(getsockname_not_bind6, getsockname_bind6);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: IPv4 UDP: getpeername (not connected)"
		 " + getpeername (connected).\n\n");
	if (!init_suite(init_udp_create_socket))
		run_suite(getpeername_not_connect, getpeername_connect);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP: getpeername (not connected).\n\n");
	if (!init_suite(init_tcp_create_socket))
		run_suite(getpeername_not_connect, getpeername_not_connect);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 TCP: getpeername (connected).\n\n");
	if (!init_suite(init_tcp_bind_listen_local_ip))
		run_suite(getpeername_connect, getpeername_accept);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 UDP: getpeername (not connected)"
		 " + getpeername (connected).\n\n");
	if (!init_suite(init_udp6_create_socket))
		run_suite(getpeername_not_connect6, getpeername_connect6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP: (not connected).\n\n");
	if (!init_suite(init_tcp6_create_socket))
		run_suite(getpeername_not_connect6, getpeername_not_connect6);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv6 TCP: getpeername (connected).\n\n");
	if (!init_suite(init_tcp6_bind_listen_local_ip))
		run_suite(getpeername_connect6, getpeername_accept6);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: gai_strerror: print error codes.\n\n");
	if (!init_suite(NULL))
		run_suite(gai_strerror_print, gai_strerror_print);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getaddrinfo NULL hints: NULL host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_null_hints_null_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: IPv4 getaddrinfo NULL hints: numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_null_hints_num_host, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: IPv6 getaddrinfo NULL hints: numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_null_hints_num6_host, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: getaddrinfo IPv4 hint, SOCK_DGRAM: NULL host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ipv4_hints_null_host, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: getaddrinfo IPv6 hint, TCP: NULL host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ipv6_hints_null_host, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: getaddrinfo AI_PASSIVE hint: NULL host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ai_passive_hints_null_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getaddrinfo AI_CANONNAME hint: numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ai_canonname_hints_num_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: getaddrinfo IPv6 hint, AI_V4MAPPED, UDP: "
		 "numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ipv6_ai_v4mapped_hints_num_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /* INET6 */

	NFP_INFO("\n\nSuite: getaddrinfo AI_NUMERICHOST hint: "
		 "numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ai_numerichost_hints_num_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getaddrinfo AI_ADDRCONFIG hint: "
		 "numeric host.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_ai_addrconfig_hints_num_host,
			  null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getaddrinfo: localhost, numeric service.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_localhost, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getaddrinfo: localhost, string service.\n\n");
	if (!init_suite(NULL))
		run_suite(getaddrinfo_localhost_service, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getservbyname: null type.\n\n");
	if (!init_suite(NULL))
		run_suite(getservbyname_null_type, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getservbyname: type.\n\n");
	if (!init_suite(NULL))
		run_suite(getservbyname_type, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getservbyport: null type.\n\n");
	if (!init_suite(NULL))
		run_suite(getservbyport_null_type, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getservbyport: type.\n\n");
	if (!init_suite(NULL))
		run_suite(getservbyport_type, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

	NFP_INFO("\n\nSuite: getnameinfo ipv4: null host, service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv4_service_only, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: getnameinfo ipv6: null host, service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv6_service_only, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: getnameinfo ipv4: host numeric, null service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv4_host_numeric_only, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: getnameinfo ipv6: host numeric, null service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv6_host_numeric_only, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	NFP_INFO("\n\nSuite: getnameinfo ipv4: host numeric, service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv4_host_numeric_service, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");

#ifdef INET6
	NFP_INFO("\n\nSuite: getnameinfo ipv6: host numeric, service.\n\n");
	if (!init_suite(NULL))
		run_suite(getnameinfo_ipv6_host_numeric_service, null_function);
	end_suite();
	NFP_INFO("Test ended.\n");
#endif /*INET6*/

	nfp_stop_processing();

	nfp_thread_join(thread_tbl, num_workers);

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed.\n");

	printf("End Main()\n");
	return 0;
}

/**
 * Configure IP addresses
 *
 */
static int configure_interface_addresses(void)
{
	const char *err;
#ifdef INET6
	uint8_t addr6[16];
#endif /*INET6*/

	err = nfp_ifport_loopback_ipv4_up(0, 0, TEST_LOOP4, TEST_LOOP4_MASK, 0);
	if (err) {
		NFP_ERR("Failed to configure IPv4 address (l0): %s", err);
		return -1;
	}

#ifdef INET6
	memcpy(addr6, TEST_LOOP6.nfp_s6_addr, TEST_LOOP6_MASK);
	err = nfp_ifport_loopback_ipv6_up(0, addr6, 64);
	if (err) {
		NFP_ERR("Failed to configure IPv6 address (l0): %s", err);
		return -1;
	}
#endif /*INET6*/

	err = nfp_ifport_net_ipv4_up(0, NFP_IFPORT_NET_SUBPORT_ITF, 0,
				     TEST_ADDR4, TEST_ADDR4_MASK, 0);
	if (err) {
		NFP_ERR("Failed to configure IPv4 address (fp0): %s", err);
		return -1;
	}

#ifdef INET6
	nfp_parse_ip6_addr(TEST_ADDR6_STR, 0, addr6);
	err = nfp_ifport_net_ipv6_up(0, NFP_IFPORT_NET_SUBPORT_ITF,
				     addr6, TEST_ADDR6_MASK, 1);
	if (err) {
		NFP_ERR("Failed to configure IPv6 address (fp0): %s", err);
		return -1;
	}
#endif /*INET6*/

	return 0;
}

/**
 * Print IP addresses
 *
 */

static int print_addr(const char *name, struct nfp_sockaddr *sa)
{
	int family = sa->sa_family;

	if (family == NFP_AF_INET) {
		struct nfp_sockaddr_in *sin = (struct nfp_sockaddr_in *)sa;

		NFP_INFO("\t\t%s: NFP_AF_INET <%s>", name,
			 nfp_print_ip_addr(sin->sin_addr.s_addr));
	} else if (family == NFP_AF_INET6) {
		struct nfp_sockaddr_in6 *sin6 = (struct nfp_sockaddr_in6 *)sa;

		NFP_INFO("\t\t%s: NFP_AF_INET6 <%s>", name,
			 nfp_print_ip6_addr(sin6->sin6_addr.nfp_s6_addr));
	} else {
		NFP_INFO("\t\t%s: unknown family", name);
	}

	return 0;
}

static int print_interfaces(void)
{
	struct nfp_ifaddrs *ifap = NULL;
	struct nfp_ifaddrs *ifap_head = NULL;

	if (nfp_getifaddrs(&ifap_head)) {
		NFP_ERR("Failed to get interface addresses: %d", nfp_errno);
		return -1;
	}

	ifap = ifap_head;
	while (ifap) {
		NFP_INFO("Interface: %-8s ", ifap->ifa_name);

		if (ifap->ifa_addr)
			print_addr("address", ifap->ifa_addr);

		if (ifap->ifa_netmask)
			print_addr("netmask", ifap->ifa_netmask);

		if (ifap->nfp_ifa_broadaddr)
			print_addr("broadcast", ifap->nfp_ifa_broadaddr);

		ifap = ifap->ifa_next;
	}

	nfp_freeifaddrs(ifap_head);
	return 0;
}

/**
 * Parse and store the command line arguments
 *
 * @param argc       argument count
 * @param argv[]     argument vector
 * @param appl_args  Store application arguments here
 */
static void parse_args(int argc, char *argv[], appl_args_t *appl_args)
{
	int opt;
	int long_index;
	char *names, *str, *token, *save;
	size_t len;
	int i;
	static struct option longopts[] = {
		{"count", required_argument, NULL, 'c'},
		{"interface", required_argument, NULL, 'i'},	/* return 'i' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"single-pkt-API", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->single_pkt_API = 0;

	while (1) {
		opt = getopt_long(argc, argv, "+c:i:hf:g",
				  longopts, &long_index);

		if (opt == -1)
			break;	/* No more options */

		switch (opt) {
		case 'c':
			appl_args->core_count = atoi(optarg);
			break;
			/* parse packet-io interface names */
		case 'i':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			len += 1;	/* add room for '\0' */

			names = malloc(len);
			if (names == NULL) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}

			/* count the number of tokens separated by ',' */
			strcpy(names, optarg);
			for (str = names, i = 0;; str = NULL, i++) {
				token = strtok_r(str, ",", &save);
				if (token == NULL)
					break;
			}
			appl_args->if_count = i;

			if (appl_args->if_count == 0) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}

			/* allocate storage for the if names */
			appl_args->if_names =
				calloc(appl_args->if_count, sizeof(char *));

			/* store the if names (reset names string) */
			strcpy(names, optarg);
			for (str = names, i = 0;; str = NULL, i++) {
				token = strtok_r(str, ",", &save);
				if (token == NULL)
					break;
				appl_args->if_names[i] = token;
			}
			break;

		case 'h':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;

		case 'f':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			len += 1;	/* add room for '\0' */

			appl_args->cli_file = malloc(len);
			if (appl_args->cli_file == NULL) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}

			strcpy(appl_args->cli_file, optarg);
			break;
		case 'g':
			appl_args->single_pkt_API = 1;
			break;

		default:
			break;
		}
	}

	if (appl_args->if_count == 0) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	optind = 1;		/* reset 'extern optind' from the getopt lib */
}

/**
 * Print system and application info
 */
static void print_info(char *progname, appl_args_t *appl_args,
		       odp_cpumask_t *cpumask)
{
	int i;
	char cpumaskstr[64];

	printf("\n"
		   "ODP system info\n"
		   "---------------\n"
		   "ODP API version: %s\n"
		   "CPU model:       %s\n"
		   "CPU freq (hz):   %"PRIu64"\n"
		   "Cache line size: %i\n"
		   "Core count:      %i\n"
		   "\n",
		   odp_version_api_str(), odp_cpu_model_str(),
		   odp_cpu_hz(), odp_sys_cache_line_size(),
		   odp_cpu_count());

	printf("Running ODP appl: \"%s\"\n"
		   "-----------------\n"
		   "IF-count:        %i\n"
		   "Using IFs:      ",
		   progname, appl_args->if_count);
	for (i = 0; i < appl_args->if_count; ++i)
		printf(" %s", appl_args->if_names[i]);
	printf("\n\n");

	printf("Packet processing API: %s\n", appl_args->single_pkt_API ?
	       "single" : "multi");
	printf("\n");

	/* Print worker to core distribution */
	if (odp_cpumask_to_str(cpumask, cpumaskstr, sizeof(cpumaskstr)) < 0) {
		printf("Error: Too small buffer provided to "
			"odp_cpumask_to_str\n");
		strcpy(cpumaskstr, "Unknown");
	}

	printf("Num worker threads: %i\n", odp_cpumask_count(cpumask));
	printf("first CPU:          %i\n", odp_cpumask_first(cpumask));
	printf("cpu mask:           %s\n", cpumaskstr);

	fflush(NULL);
}

/**
 * Prinf usage information
 */
static void usage(char *progname)
{
	printf("\n"
		   "Usage: %s OPTIONS\n"
		   "  E.g. %s -i eth1,eth2,eth3\n"
		   "\n"
		   "ODPFastpath application.\n"
		   "\n"
		   "Mandatory OPTIONS:\n"
		   "  -i, --interface Eth interfaces (comma-separated, no spaces)\n"
		   "\n"
		   "Optional OPTIONS\n"
		   "  -c, --count <number> Core count.\n"
		   "  -f, --cli-file <file> NFP CLI file - overrides default IP configuration.\n"
		   "  -g, --single-pkt-API Use single packet processing API\n"
		   "  -h, --help           Display help and exit.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname)
		);
}
