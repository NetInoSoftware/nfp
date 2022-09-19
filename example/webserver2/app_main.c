/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "nfp.h"
#include "linux_sigaction.h"
#include "linux_resources.h"
#include "cli_arg_parse.h"
#include "httpd.h"

#define MAX_WORKERS		32
#define PKT_BURST_SIZE 32

/** Get rid of path in filename - only for unix-type paths using '/' */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

enum execution_mode {
	EXEC_MODE_SCHEDULER = 0,
	EXEC_MODE_DIRECT_RSS,
	EXEC_MODE_SCHEDULER_RSS,
	EXEC_MODE_MAX = EXEC_MODE_SCHEDULER_RSS
};

/**
 * Parsed command line application arguments
 */
typedef struct {
	enum execution_mode mode;
	int core_count;
	int core_start;
	appl_arg_ifs_t itf_param;
	char *cli_file;
	char *root_dir;
	char *laddr;
	uint16_t lport;
	odp_bool_t single_pkt_API;
} appl_args_t;

struct worker_arg {
	int num_pktin;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	odp_bool_t sched_process;
};

struct interface_id {
	int port;
	uint16_t subport;
};

/* helper funcs */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args);
static void parse_args_cleanup(appl_args_t *appl_args);
static void print_info(char *progname, appl_args_t *appl_args);
static void usage(char *progname);
static int validate_cores_settings(int req_core_start, int req_core_count,
	int *core_start, int *core_count);

static int handle_sched_events(void)
{
	odp_event_t events[PKT_BURST_SIZE], ev;
	odp_queue_t in_queue;
	int event_cnt, i;

	event_cnt = odp_schedule_multi(&in_queue, ODP_SCHED_NO_WAIT,
				       events, PKT_BURST_SIZE);
	for (i = 0; i < event_cnt; i++) {
		ev = events[i];

		if (ev == ODP_EVENT_INVALID)
			continue;

		if (odp_event_type(ev) == ODP_EVENT_TIMEOUT)
			nfp_timer_handle(ev);
		else if (odp_event_type(ev) == ODP_EVENT_PACKET)
			nfp_packet_input(odp_packet_from_event(ev),
					 in_queue, /* loopbacked, SP pkts */
					 nfp_eth_vlan_processing);
		else
			odp_buffer_free(odp_buffer_from_event(ev));
	}

	return 0;
}

/** pkt_io_direct_mode_recv() Custom event dispatcher
 *
 * @param arg void*  Worker argument
 * @return int Never returns
 *
 */
static int pkt_io_direct_mode_recv(void *arg)
{
	odp_packet_t pkt_tbl[PKT_BURST_SIZE], pkt;
	int pkt_idx, pkt_cnt;
	struct worker_arg *thr_args;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t *is_running = NULL;
	nfp_pkt_vector_t vec;
	odp_bool_t sched_process;

	thr_args = arg;
	num_pktin = thr_args->num_pktin;
	sched_process = thr_args->sched_process;

	for (i = 0; i < num_pktin; i++)
		pktin[i] = thr_args->pktin[i];

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	ptr = (uint8_t *)&pktin[0];

	nfp_pkt_vector_init(&vec);

	printf("PKT-IO receive starting on cpu: %i, %i, %x:%x\n", odp_cpu_id(),
	       num_pktin, ptr[0], ptr[8]);

	while (*is_running) {
		if (sched_process)
			handle_sched_events();

		for (i = 0; i < num_pktin; i++) {
			pkt_cnt = odp_pktin_recv(pktin[i], pkt_tbl,
						 PKT_BURST_SIZE);

			for (pkt_idx = 0; pkt_idx < pkt_cnt; pkt_idx++) {
				pkt = pkt_tbl[pkt_idx];

				if (nfp_pkt_vector_add(&vec, pkt) != 1) {
					odp_packet_free(pkt);
					continue;
				}

				if (NFP_PKT_VECTOR_IS_FULL(&vec)) {
					nfp_packet_input_multi(&vec, ODP_QUEUE_INVALID, nfp_eth_vlan_processing_multi);

					nfp_pkt_vector_init(&vec);
				}
			}
		}
		if (NFP_PKT_VECTOR_HAS_DATA(&vec)) {
			nfp_packet_input_multi(&vec, ODP_QUEUE_INVALID,
					       nfp_eth_vlan_processing_multi);

			nfp_pkt_vector_init(&vec);
		}
		nfp_send_pending_pkt();
	}

	/* Never reached */
	return 0;
}

static int pkt_io_direct_mode_recv_single(void *arg)
{
	odp_packet_t pkt_tbl[PKT_BURST_SIZE], pkt;
	int pkt_idx, pkt_cnt;
	struct worker_arg *thr_args;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t *is_running = NULL;
	odp_bool_t sched_process;

	thr_args = arg;
	num_pktin = thr_args->num_pktin;
	sched_process = thr_args->sched_process;

	for (i = 0; i < num_pktin; i++)
		pktin[i] = thr_args->pktin[i];

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	ptr = (uint8_t *)&pktin[0];

	printf("PKT-IO receive starting on cpu: %i, %i, %x:%x\n", odp_cpu_id(),
	       num_pktin, ptr[0], ptr[8]);

	while (*is_running) {
		if (sched_process)
			handle_sched_events();

		for (i = 0; i < num_pktin; i++) {
			pkt_cnt = odp_pktin_recv(pktin[i], pkt_tbl,
						 PKT_BURST_SIZE);

			for (pkt_idx = 0; pkt_idx < pkt_cnt; pkt_idx++) {
				pkt = pkt_tbl[pkt_idx];

				nfp_packet_input(pkt, ODP_QUEUE_INVALID,
					nfp_eth_vlan_processing);
			}
		}
		nfp_send_pending_pkt();
	}

	/* Never reached */
	return 0;
}

/** create_interfaces_direct_rss() Create NFP interfaces with
 * pktios open in direct mode, thread unsafe and using RSS with
 * hashing by IPv4 addresses and TCP ports
 *
 * @param itf_param appl_arg_ifs_t Interfaces to configure
 * @param tx_queue int Number of requested transmision queues
 *    per interface
 * @param rx_queue int Number of requested reciver queues per
 *    interface
 * @param itf_id struct interface_id IDs (port and subport) of
 *    the configured interfaces
 * @return int 0 on success, -1 on error
 *
 */
static int create_interfaces_direct_rss(appl_arg_ifs_t *itf_param,
					int tx_queues, int rx_queues,
					struct interface_id *itf_id)
{
	nfp_ifport_net_param_t if_param;
	odp_pktio_param_t pktio_param;
	odp_pktin_queue_param_t pktin_param;
	odp_pktout_queue_param_t pktout_param;
	int i;

	nfp_ifport_net_param_init(&if_param);
	if_param.pktio_param = &pktio_param;
	if_param.pktin_param = &pktin_param;
	if_param.pktout_param = &pktout_param;
	if_param.if_sp_mgmt = 1;

	odp_pktio_param_init(&pktio_param);
	pktio_param.in_mode = ODP_PKTIN_MODE_DIRECT;
	pktio_param.out_mode = ODP_PKTOUT_MODE_DIRECT;

	odp_pktin_queue_param_init(&pktin_param);
	pktin_param.op_mode = ODP_PKTIO_OP_MT_UNSAFE;
	pktin_param.hash_enable = 1;
	pktin_param.hash_proto.proto.ipv4_tcp = 1;
	pktin_param.hash_proto.proto.ipv4_udp = 1;
	pktin_param.num_queues = rx_queues;

	odp_pktout_queue_param_init(&pktout_param);
	pktout_param.op_mode = ODP_PKTIO_OP_MT; /* see note below */
	pktout_param.num_queues = tx_queues;

	/** Note: Using output queues as multithread unsafe
	 * (ODP_PKTIO_OP_MT_UNSAFE) is tricky and should be avoided.
	 */

	for (i = 0; i < itf_param->if_count; i++)
		if (nfp_ifport_net_create(itf_param->if_array[i].if_name,
					  &if_param,
					  &itf_id[i].port,
					  &itf_id[i].subport) < 0) {
			NFP_ERR("Failed to init interface %s",
				itf_param->if_array[i].if_name);
			return -1;
		}

	return 0;
}

/** configure_workers_arg_direct_rss() Configure workers
 *  argument
 *
 * @param itf_param appl_arg_ifs_t Interfaces parameters
 * @param num_workers int  Number of workers
 * @param workers_arg struct worker_arg* Array of workers
 *    argument
 * @return int 0 on success, -1 on error
 *
 */
static int configure_workers_arg_direct_rss(appl_arg_ifs_t *itf_param,
					    int num_workers,
					    struct worker_arg *workers_arg)
{
	int i, j;
	odp_pktio_t pktio;
	odp_pktin_queue_t pktin[MAX_WORKERS];

	for (i = 0; i < num_workers; i++) {
		workers_arg[i].num_pktin = itf_param->if_count;
		workers_arg[i].sched_process = i ? 0 : 1;
	}


	for (i = 0; i < itf_param->if_count; i++) {
		pktio = odp_pktio_lookup(itf_param->if_array[i].if_name);
		if (pktio == ODP_PKTIO_INVALID) {
			NFP_ERR("Failed locate pktio %s",
				itf_param->if_array[i].if_name);
			return -1;
		}

		if (odp_pktin_queue(pktio, pktin, num_workers) != num_workers) {
			NFP_ERR("Too few pktin queues for %s",
				itf_param->if_array[i].if_name);
			return -1;
		}

		for (j = 0; j < num_workers; j++)
			workers_arg[j].pktin[i] = pktin[j];
	}

	return 0;
}


/** create_interfaces_sched_rss() Create NFP interfaces with
 * pktios open in  scheduler mode and using RSS
 * with hashing by IPv4 addresses and TCP ports
 *
 * @param itf_param appl_arg_ifs_t Interfaces to configure
 * @param tx_queue int Number of requested transmision queues
 *    per interface
 * @param rx_queue int Number of requested reciver queues per
 *    interface
 * @param itf_id struct interface_id IDs (port and subport) of
 *    the configured interfaces
 * @return int 0 on success, -1 on error
 *
 */
static int create_interfaces_sched_rss(appl_arg_ifs_t *itf_param,
				       int tx_queues, int rx_queues,
					   struct interface_id *itf_id)
{
	nfp_ifport_net_param_t if_param;
	odp_pktio_param_t pktio_param;
	odp_pktin_queue_param_t pktin_param;
	odp_pktout_queue_param_t pktout_param;
	int i;

	nfp_ifport_net_param_init(&if_param);
	if_param.pktio_param = &pktio_param;
	if_param.pktin_param = &pktin_param;
	if_param.pktout_param = &pktout_param;
	if_param.if_sp_mgmt = 1;

	odp_pktio_param_init(&pktio_param);
	pktio_param.in_mode = ODP_PKTIN_MODE_SCHED;
	pktio_param.out_mode = ODP_PKTOUT_MODE_DIRECT;

	odp_pktin_queue_param_init(&pktin_param);
	pktin_param.op_mode = ODP_PKTIO_OP_MT;
	pktin_param.classifier_enable = 0;
	pktin_param.hash_enable = 1;
	pktin_param.hash_proto.proto.ipv4_tcp = 1;
	pktin_param.hash_proto.proto.ipv4_udp = 1;
	pktin_param.num_queues = rx_queues;
	pktin_param.queue_param.type = ODP_QUEUE_TYPE_SCHED;
	pktin_param.queue_param.enq_mode = ODP_QUEUE_OP_MT;
	pktin_param.queue_param.deq_mode = ODP_QUEUE_OP_MT;
	pktin_param.queue_param.context = NULL;
	pktin_param.queue_param.context_len = 0;
	pktin_param.queue_param.sched.prio = odp_schedule_default_prio();
	pktin_param.queue_param.sched.sync = ODP_SCHED_SYNC_ATOMIC;
	pktin_param.queue_param.sched.group = ODP_SCHED_GROUP_ALL;
	pktin_param.queue_param.sched.lock_count = 0;

	odp_pktout_queue_param_init(&pktout_param);
	pktout_param.op_mode    = ODP_PKTIO_OP_MT; /* see note below */
	pktout_param.num_queues = tx_queues;

	/** Note: Using output queues as multithread unsafe
	 * (ODP_PKTIO_OP_MT_UNSAFE) is tricky and should be avoided.
	 */

	for (i = 0; i < itf_param->if_count; i++)
		if (nfp_ifport_net_create(itf_param->if_array[i].if_name,
					  &if_param,
					  &itf_id[i].port,
					  &itf_id[i].subport) < 0) {
			NFP_ERR("Failed to init interface %s",
				itf_param->if_array[i].if_name);
			return -1;
		}

	return 0;
}

/** get_interfaces_id() Get interfaces IDs by name
 *
 * @param itf_param appl_arg_ifs_t Interfaces parameters
 * @param itf_id struct interface_id IDs (port and subport) of
 *     the configured interfaces
 * @return int 0 on success, -1 on error
 *
 */
static int get_interfaces_id(appl_arg_ifs_t *itf_param,
			     struct interface_id *itf_id)
{
	struct appl_arg_if *ifarg = NULL;
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	int i, ret = 0;

	for (i = 0; i < itf_param->if_count; i++) {
		ifarg = &itf_param->if_array[i];

		if (!ifarg->if_name) {
			NFP_ERR("Error: Invalid interface name: null");
			ret = -1;
			break;
		}

		if (!ifarg->if_address)
			continue; /* Not set through application parameters*/

		ifnet = nfp_ifport_net_ifnet_get_by_name(ifarg->if_name);
		if (ifnet == NFP_IFNET_INVALID) {
			NFP_ERR("Error: interface not found: %s",
				ifarg->if_name);
			ret = -1;
			break;
		}

		if (nfp_ifnet_port_get(ifnet,
				       &itf_id[i].port,
				       &itf_id[i].subport)) {
			NFP_ERR("Error: Failed to get <port, sub-port>: %s",
				ifarg->if_name);
			ret = -1;
			break;
		}
	}

	return ret;
}

/** configure_interface_addresses() Configure IPv4 addresses
 *
 * @param itf_param appl_arg_ifs_t Interfaces to configure
 * @param itf_id struct interface_id IDs (port and subport) of
 *     the configured interfaces
 * @return int 0 on success, -1 on error
 *
 */
static int configure_interface_addresses(appl_arg_ifs_t *itf_param,
					 struct interface_id *itf_id)
{
	struct appl_arg_if *ifarg = NULL;
	uint32_t addr = 0;
	int i, ret = 0;
	const char *res = NULL;

	for (i = 0; i < itf_param->if_count && i < NFP_FP_INTERFACE_MAX; i++) {
		ifarg = &itf_param->if_array[i];

		if (!ifarg->if_name) {
			NFP_ERR("Error: Invalid interface name: null");
			ret = -1;
			break;
		}

		if (!ifarg->if_address)
			continue; /* Not set through application parameters*/

		NFP_DBG("Setting %s/%d on %s", ifarg->if_address,
			ifarg->if_address_masklen, ifarg->if_name);

		if (!nfp_parse_ip_addr(ifarg->if_address, &addr)) {
			NFP_ERR("Error: Failed to parse IPv4 address: %s",
				ifarg->if_address);
			ret = -1;
			break;
		}

		res = nfp_ifport_net_ipv4_up(itf_id[i].port, itf_id[i].subport,
					     0,
					     addr, ifarg->if_address_masklen,
					     1);
		if (res != NULL) {
			NFP_ERR("Error: Failed to set IPv4 address %s "
				"on interface %s: %s",
				ifarg->if_address, ifarg->if_name, res);
			ret = -1;
			break;
		}
	}

	return ret;
}


/**
 * main() Application entry point
 *
 * This is the main function of the application. See 'usage'
 * function for available arguments and usage.
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
	int num_workers, first_worker, linux_sp_core, i;
	struct worker_arg workers_arg_direct_rss[MAX_WORKERS];
	struct interface_id itf_id[NFP_FP_INTERFACE_MAX];
	odp_cpumask_t cpu_mask;

	/* Setup system resources */
	nfpexpl_resources_set();

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse application arguments */
	if (parse_args(argc, argv, &params) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/*
	 * This example creates a custom workers to cores distribution:
	 * Core #0 runs Slow Path background tasks.
	 * Cores #core_start and beyond run packet processing tasks.
	 * It is recommanded to start mapping threads from core 1. Else,
	 * Slow Path processing will be affected by workers processing.
	 * However, if Slow Path is disabled, core 0 may be used as well.
	 */
	linux_sp_core = 0;

	/* Initialize NFP*/
	nfp_initialize_param(&app_init_params);
	app_init_params.cli.os_thread.start_on_init = 1;
	app_init_params.linux_core_id = linux_sp_core;
	if (params.mode == EXEC_MODE_SCHEDULER) {
		app_init_params.if_count = params.itf_param.if_count;
		for (i = 0; i < params.itf_param.if_count &&
		     i < NFP_FP_INTERFACE_MAX; i++) {
			strncpy(app_init_params.if_names[i],
				params.itf_param.if_array[i].if_name,
				NFP_IFNAMSIZ);
			app_init_params.if_names[i][NFP_IFNAMSIZ - 1] = '\0';
		}
	}

	if (nfp_initialize(&app_init_params)) {
		NFP_ERR("Error: NFP global init failed.\n");
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	/* Validate workers distribution settings. */
	if (validate_cores_settings(params.core_start, params.core_count,
				    &first_worker, &num_workers) < 0) {
		nfp_terminate();
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params);

	NFP_INFO("SP core: %d\nWorkers core start: %d\n"
		"Workers core count: %d\n",
		linux_sp_core, first_worker, num_workers);

	odp_memset(itf_id, 0, sizeof(itf_id));
	if (params.mode == EXEC_MODE_DIRECT_RSS) {
		if (create_interfaces_direct_rss(&params.itf_param,
						 num_workers + 1, num_workers,
						 itf_id)) {
			NFP_ERR("Failed to initialize interfaces.");
			nfp_terminate();
			parse_args_cleanup(&params);
			exit(EXIT_FAILURE);
		}

		if (configure_workers_arg_direct_rss(&params.itf_param,
						     num_workers,
						     workers_arg_direct_rss)) {
			NFP_ERR("Failed to initialize workers arguments.");
			nfp_terminate();
			parse_args_cleanup(&params);
			exit(EXIT_FAILURE);
		}
	} else if (params.mode == EXEC_MODE_SCHEDULER_RSS) {
		if (create_interfaces_sched_rss(&params.itf_param,
						num_workers + 1, num_workers,
						itf_id)) {
			NFP_ERR("Failed to initialize interfaces.");
			nfp_terminate();
			parse_args_cleanup(&params);
			exit(EXIT_FAILURE);
		}
	} else if (params.mode == EXEC_MODE_SCHEDULER) {
		if (get_interfaces_id(&params.itf_param, itf_id)) {
			NFP_ERR("Failed to get the interfaces IDs.");
			nfp_terminate();
			parse_args_cleanup(&params);
			exit(EXIT_FAILURE);
		}
	} else {
		NFP_ERR("Unknown execution mode.");
		nfp_terminate();
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	/* Create worker threads */
	memset(thread_tbl, 0, sizeof(thread_tbl));
	for (i = 0; i < num_workers; i++) {
		nfp_thread_param_init(&thread_param);

		if (params.mode == EXEC_MODE_DIRECT_RSS) {
			if (params.single_pkt_API)
				thread_param.start = pkt_io_direct_mode_recv_single;
			else
				thread_param.start = pkt_io_direct_mode_recv;
			thread_param.arg = &workers_arg_direct_rss[i];
		} else {
			if (params.single_pkt_API) {
				thread_param.start = default_event_dispatcher;
				thread_param.arg = nfp_eth_vlan_processing;
			} else {
				thread_param.start = default_event_dispatcher_multi;
				thread_param.arg = nfp_eth_vlan_processing_multi;
			}
		}
		thread_param.thr_type = ODP_THREAD_WORKER;

		odp_cpumask_zero(&cpu_mask);
		odp_cpumask_set(&cpu_mask, first_worker + i);

		if (nfp_thread_create(&thread_tbl[i], 1, &cpu_mask,
				      &thread_param) != 1)
			break;
	}

	if (i < num_workers) {
		NFP_ERR("Error: Failed to create worker threads, "
			"expected %d, got %d", num_workers, i);
		nfp_stop_processing();
		if (i > 0)
			nfp_thread_join(thread_tbl, i);
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/* Configure IP addresses */
	if (configure_interface_addresses(&params.itf_param, itf_id)) {
		NFP_ERR("Error: Failed to configure addresses");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/*
	 * Process the CLI commands file (if defined).
	 * This is an alternative way to set the IP addresses and other
	 * parameters.
	 */
	if (nfp_cli_process_file(params.cli_file)) {
		NFP_ERR("Error: Failed to process CLI file.");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}
	sleep(2);

	/* webserver */
	if (setup_webserver(params.root_dir, params.laddr, params.lport)) {
		NFP_ERR("Error: Failed to setup webserver.");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		nfp_terminate();
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	nfp_thread_join(thread_tbl, num_workers);

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed.\n");

	parse_args_cleanup(&params);
	printf("End Main()\n");
	return 0;
}

/**
 * validate_cores_settings() Validate requested core settings
 * and computed actual values
 *
 *
 * @param req_core_start int Requested worker core start
 * @param req_core_count int Requested worker core count
 * @param core_start int* Computed worker core start
 * @param core_count int* Computed worker core count
 * @return int 0 on success, -1 on error
 *
 */
static int validate_cores_settings(int req_core_start, int req_core_count,
	 int *core_start, int *core_count)
{
	int total_core_count = odp_cpu_count();

	if (req_core_start >= total_core_count) {
		NFP_ERR("ERROR: Invalid 'core start' parameter: %d. Max = %d\n",
			req_core_start, total_core_count - 1);
		return -1;
	}
	*core_start = req_core_start;

	if (req_core_count) {
		if (*core_start + req_core_count > total_core_count) {
			NFP_ERR("ERROR: Invalid 'core start' 'core count' "
				"configuration: %d,%d\n"
				"Exeeds number of avilable cores: %d",
				*core_start, req_core_count, total_core_count);
			return -1;
		}
		*core_count = req_core_count;
	} else
		*core_count = total_core_count - *core_start;

	if (*core_count < 0) {
		NFP_ERR("ERROR: At least 1 core is required.\n");
		return -1;
	}
	if (*core_count > MAX_WORKERS)  {
		NFP_ERR("ERROR: Number of processing cores %d"
			" exeeds maximum number for this test %d.\n",
			*core_count, MAX_WORKERS);
		return -1;
	}
	return 0;
}
/**
 * Parse and store the command line arguments
 *
 * @param argc       argument count
 * @param argv[]     argument vector
 * @param appl_args  Store application arguments here
 * @return int 0 on success, -1 on error
 */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args)
{
	int opt, res = 0;
	int long_index;
	size_t len;
	static struct option longopts[] = {
		{"mode", required_argument, NULL, 'm'},
		{"core_count", required_argument, NULL, 'c'},
		{"core_start", required_argument, NULL, 's'},
		{"interface", required_argument, NULL, 'i'},	/* return 'i' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"root", required_argument, NULL, 'r'},	/* return 'r' */
		{"laddr", required_argument, NULL, 'l'},	/* return 'l' */
		{"lport", required_argument, NULL, 'p'},	/* return 'p' */
		{"single-pkt-API", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->core_start = 1;
	appl_args->core_count = 0; /* all above core start */
	appl_args->single_pkt_API = 0;

	while (res == 0) {
		opt = getopt_long(argc, argv, "+c:s:i:hf:r:l:p:m:g",
				  longopts, &long_index);

		if (opt == -1)
			break;	/* No more options */

		switch (opt) {
		case 'm':
		{
			int mode_arg = atoi(optarg);

			if (mode_arg < 0 || mode_arg > EXEC_MODE_MAX) {
				usage(argv[0]);
				res = -1;
				break;
			}
			appl_args->mode = mode_arg;
			break;
		}
		case 'c':
			appl_args->core_count = atoi(optarg);
			break;
		case 's':
			appl_args->core_start = atoi(optarg);
			break;
			/* parse packet-io interface names */
		case 'i':
			res = nfpexpl_parse_interfaces(optarg,
						       &appl_args->itf_param);
			if (res == EXIT_FAILURE) {
				usage(argv[0]);
				res = -1;
			}
			break;

		case 'h':
			usage(argv[0]);
			parse_args_cleanup(appl_args);
			exit(EXIT_SUCCESS);
			break;

		case 'f':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			len += 1;	/* add room for '\0' */

			appl_args->cli_file = malloc(len);
			if (appl_args->cli_file == NULL) {
				usage(argv[0]);
				res = -1;
				break;
			}

			strcpy(appl_args->cli_file, optarg);
			break;
		case 'r':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			len += 1;	/* add room for '\0' */

			appl_args->root_dir = malloc(len);
			if (appl_args->root_dir == NULL) {
				usage(argv[0]);
				res = -1;
				break;
			}

			strcpy(appl_args->root_dir, optarg);
			break;
		case 'l':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			len += 1;	/* add room for '\0' */

			appl_args->laddr = malloc(len);
			if (appl_args->laddr == NULL) {
				usage(argv[0]);
				res = -1;
				break;
			}

			strcpy(appl_args->laddr, optarg);
			break;
		case 'p':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			len += 1;	/* add room for '\0' */

			appl_args->lport = (uint64_t)atoi(optarg);
			break;
		case 'g':
			appl_args->single_pkt_API = 1;
			break;
		default:
			break;
		}
	}

	if (res == -1) {
		parse_args_cleanup(appl_args);
		return EXIT_FAILURE;
	}

	if (appl_args->itf_param.if_count == 0) {
		usage(argv[0]);
		parse_args_cleanup(appl_args);
		return EXIT_FAILURE;
	}

	if (appl_args->itf_param.if_count > NFP_FP_INTERFACE_MAX) {
		printf("Error: Invalid number of interfaces: maximum %d\n",
		       NFP_FP_INTERFACE_MAX);
		parse_args_cleanup(appl_args);
		return EXIT_FAILURE;
	}

	optind = 1;		/* reset 'extern optind' from the getopt lib */
	return EXIT_SUCCESS;
}

/**
 * Cleanup the stored command line arguments
 *
 * @param appl_args  application arguments
 */
static void parse_args_cleanup(appl_args_t *appl_args)
{
	nfpexpl_parse_interfaces_param_cleanup(&appl_args->itf_param);
}

/**
 * Print system and application info
 */
static void print_info(char *progname, appl_args_t *appl_args)
{
	int i;

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
		   progname, appl_args->itf_param.if_count);
	for (i = 0; i < appl_args->itf_param.if_count; ++i)
		printf(" %s", appl_args->itf_param.if_array[i].if_name);
	printf("\n");

	printf("Execution mode: ");
	switch (appl_args->mode) {
	case EXEC_MODE_DIRECT_RSS:
		printf("direct_rss");
		break;
	case EXEC_MODE_SCHEDULER_RSS:
		printf("sched_rss");
		break;
	default:
		printf("scheduler");
	}
	printf("\n");
	printf("Packet processing API: %s\n", appl_args->single_pkt_API ?
	       "single" : "multi");
	printf("\n");
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
		   "  -i, --interface <interfaces> Ethernet interface list"
		   " (comma-separated, no spaces)\n"
		   "  Example:\n"
		   "    eth1,eth2\n"
		   "    eth1@192.168.100.10/24,eth2@172.24.200.10/16\n"
		   "\n"
		   "Optional OPTIONS\n"
		   "  -m, --mode <number> Execution mode:\n"
		   "          0 scheduler mode (single input queue),\n"
		   "          1 direct_rss mode (multiple input queue),\n"
		   "          2 sched_rss (multiple input queue). Default 0.\n"
		   "  -c, --core_count <number> Core count. Default 0: all above core start\n"
		   "  -s, --core_start <number> Core start. Default 1.\n"
		   "  -r, --root <web root folder> Webserver root folder.\n"
		   "\tDefault: " DEFAULT_ROOT_DIRECTORY "\n"
		   "  -f, --cli-file <file> NFP CLI file.\n"
		   "  -l, --laddr <IPv4 address> IPv4 address were webserver binds.\n"
			"\tDefault: %s\n"
		   "  -p, --lport <port> Port address were webserver binds.\n"
			"\tDefault: %d\n"
		   "  -g, --single-pkt-API Use single packet processing API\n"
		   "  -h, --help           Display help and exit.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname),
		   nfp_print_ip_addr(DEFAULT_BIND_ADDRESS), DEFAULT_BIND_PORT
		);
}
