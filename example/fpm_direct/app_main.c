/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <getopt.h>
#include <string.h>
#include <inttypes.h>

#include "nfp.h"
#include "linux_sigaction.h"
#include "cli_arg_parse.h"

#define MAX_WORKERS		64
#define PKT_BURST_SIZE NFP_PKT_VECTOR_SIZE

/** Get rid of path in filename - only for unix-type paths using '/' */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

/**
 * Parsed command line application arguments
 */
typedef struct {
	int core_count;
	int core_start;
	appl_arg_ifs_t itf_param;
	char *cli_file;
	odp_bool_t single_pkt_API;
} appl_args_t;

struct worker_arg {
	int num_pktin;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	odp_bool_t process_timers;
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

/** pkt_io_recv_single() Custom event dispatcher
 *
 * @param _arg void*  Worker argument
 * @return int Never returns
 *
 */
static int pkt_io_recv_single(void *_arg)
{
	odp_queue_t in_queue;
	odp_packet_t pkt, pkt_tbl[PKT_BURST_SIZE];
	odp_event_t events[PKT_BURST_SIZE], ev;
	int pkt_idx, pkt_cnt, event_cnt;
	struct worker_arg *arg;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t process_timers;
	odp_bool_t *is_running = NULL;

	arg = (struct worker_arg *)_arg;
	process_timers = arg->process_timers;
	num_pktin = arg->num_pktin;

	for (i = 0; i < num_pktin; i++)
		pktin[i] = arg->pktin[i];

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	ptr = (uint8_t *)&pktin[0];

	printf("PKT-IO receive starting on cpu: %i, %i, %x:%x\n", odp_cpu_id(),
	       num_pktin, ptr[0], ptr[8]);

	while (*is_running) {
		if (process_timers) {
			event_cnt = odp_schedule_multi(&in_queue,
						       ODP_SCHED_NO_WAIT,
						       events, PKT_BURST_SIZE);
			for (i = 0; i < event_cnt; i++) {
				ev = events[i];

				if (ev == ODP_EVENT_INVALID)
					continue;

				if (odp_event_type(ev) == ODP_EVENT_TIMEOUT)
					nfp_timer_handle(ev);
				else if (odp_event_type(ev) == ODP_EVENT_PACKET)
					nfp_packet_input(/*loopbacked, SP pkts*/
						odp_packet_from_event(ev),
						in_queue,
						nfp_eth_vlan_processing);
				else
					odp_buffer_free(
						odp_buffer_from_event(ev));
			}
		}
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

/** pkt_io_recv() Custom event dispatcher
 *
 * @param _arg void*  Worker argument
 * @return int Never returns
 *
 */
static int pkt_io_recv(void *_arg)
{
	odp_queue_t in_queue;
	odp_packet_t pkt, pkt_tbl[PKT_BURST_SIZE];
	odp_event_t events[PKT_BURST_SIZE], ev;
	int pkt_idx, pkt_cnt, event_cnt;
	struct worker_arg *arg;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t process_timers;
	odp_bool_t *is_running = NULL;
	nfp_pkt_vector_t vec;

	arg = (struct worker_arg *)_arg;
	process_timers = arg->process_timers;
	num_pktin = arg->num_pktin;

	for (i = 0; i < num_pktin; i++)
		pktin[i] = arg->pktin[i];

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
		if (process_timers) {
			event_cnt = odp_schedule_multi(&in_queue,
						       ODP_SCHED_NO_WAIT,
						       events, PKT_BURST_SIZE);
			for (i = 0; i < event_cnt; i++) {
				ev = events[i];

				if (ev == ODP_EVENT_INVALID)
					continue;

				if (odp_event_type(ev) == ODP_EVENT_TIMEOUT) {
					nfp_timer_handle(ev);
					continue;
				} else if (odp_event_type(ev) ==
						ODP_EVENT_PACKET) {
					nfp_packet_input(/*loopbacked, SP pkts*/
						odp_packet_from_event(ev),
						in_queue,
						nfp_eth_vlan_processing);
					continue;
				}

				odp_buffer_free(odp_buffer_from_event(ev));
			}
		}
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
					nfp_packet_input_multi(&vec, ODP_QUEUE_INVALID,
							       nfp_eth_vlan_processing_multi);

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

/** configure_interfaces() Create NFP interfaces with
 * pktios open in direct mode, thread unsafe.
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
static int configure_interfaces(appl_arg_ifs_t *itf_param,
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
	pktin_param.hash_proto.proto.ipv4_udp = 1;
	pktin_param.hash_proto.proto.ipv4_tcp = 1;
	pktin_param.hash_proto.proto.ipv6_udp = 1;
	pktin_param.num_queues = rx_queues;

	odp_pktout_queue_param_init(&pktout_param);
	pktout_param.op_mode    = ODP_PKTIO_OP_MT; /* see note below */
	pktout_param.num_queues = tx_queues;

/** Note: Using output queues as multithread unsafe (ODP_PKTIO_OP_MT_UNSAFE) is
 * tricky and should be avoided unless good understanding of output queues
 * selection algorithm and thread to core allocation:
 *
 * out_queue_idx = core_id % tx_queues_count
 *
 * Valid configuration:
 * linux_core_id = 0
 * workers are set on cores 1 - N
 * tx_queues_count = N + 1
 *
 * Other configurations may result in multiple threads using the same output
 * queue in MT unsafe mode resulting in packet send failures.
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

/** configure_workers_arg() Configure workers
 *  argument
 *
 * @param num_workers int  Number of workers
 * @param workers_arg struct worker_arg* Array of workers
 *    argument
 * @param if_count int  Interface count
 * @param if_names char** Interface names
 * @return int 0 on success, -1 on error
 *
 */
static int configure_workers_arg(appl_arg_ifs_t *itf_param,
				 int num_workers,
				 struct worker_arg *workers_arg)
{
	odp_pktio_t pktio;
	odp_pktin_queue_t pktin[MAX_WORKERS];
	int i, j;

	for (i = 0; i < num_workers; i++) {
		workers_arg[i].num_pktin = itf_param->if_count;
		workers_arg[i].process_timers = 0;
	}
	/*enable timer processing on first core*/
	workers_arg[0].process_timers = 1;

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
			exit(EXIT_FAILURE);
		}

		for (j = 0; j < num_workers; j++)
			workers_arg[j].pktin[i] = pktin[j];
	}

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
	struct worker_arg workers_arg[MAX_WORKERS];
	struct interface_id itf_id[NFP_FP_INTERFACE_MAX];
	int num_workers, first_worker, linux_sp_core, i, ret_val;
	odp_cpumask_t cpu_mask;

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse and store the application arguments */
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
	app_init_params.evt_rx_burst_size = NFP_PKT_VECTOR_SIZE;
	app_init_params.pkt_tx_burst_size = NFP_PKT_VECTOR_SIZE;
	app_init_params.linux_core_id = linux_sp_core;
	app_init_params.cli.os_thread.start_on_init = 1;

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
	if (configure_interfaces(&params.itf_param, /*one tx queue for SP core*/
				 num_workers + 1, num_workers, itf_id)) {
		NFP_ERR("Error: Failed to configure interfaces.\n");
		nfp_terminate();
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	if (configure_workers_arg(&params.itf_param,
				  num_workers, workers_arg)) {
		NFP_ERR("Failed to initialize workers arguments.");
		nfp_terminate();
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	/* Create worker threads */
	memset(thread_tbl, 0, sizeof(thread_tbl));
	for (i = 0; i < num_workers; ++i) {
		nfp_thread_param_init(&thread_param);
		if (params.single_pkt_API)
			thread_param.start = pkt_io_recv_single;
		else
			thread_param.start = pkt_io_recv;
		thread_param.arg = &workers_arg[i];
		thread_param.thr_type = ODP_THREAD_WORKER;

		odp_cpumask_zero(&cpu_mask);
		odp_cpumask_set(&cpu_mask, first_worker + i);

		ret_val = nfp_thread_create(&thread_tbl[i], 1, &cpu_mask,
					    &thread_param);
		if (ret_val != 1) {
			NFP_ERR("Error: Failed to create worker threads, "
				"expected %d, got %d",
				num_workers, i);
			nfp_stop_processing();
			nfp_thread_join(thread_tbl, i);
			nfp_terminate();
			parse_args_cleanup(&params);
			return EXIT_FAILURE;
		}
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
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args)
{
	int opt, res = 0;
	int long_index;
	size_t len;
	static struct option longopts[] = {
		{"core_count", required_argument, NULL, 'c'},
		{"core_start", required_argument, NULL, 's'},
		{"interface", required_argument, NULL, 'i'},	/* return 'i' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"single-pkt-API", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->core_start = 1;
	appl_args->core_count = 0; /* all above core start */
	appl_args->single_pkt_API = 0;

	while (res == 0) {
		opt = getopt_long(argc, argv, "+c:s:i:hf:g",
				  longopts, &long_index);

		if (opt == -1)
			break;	/* No more options */

		switch (opt) {
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
	printf("\n\n");

	printf("Packet processing API: %s\n\n", appl_args->single_pkt_API ?
	       "single" : "multi");

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
		   "  -s, --core_start <number> Core start. Default 1.\n"
		   "  -c, --core_count <number> Core count. Default 0: all above core start\n"
		   "  -f, --cli-file <file> NFP CLI file.\n"
		   "  -g, --single-pkt-API  Use single packet processing API\n"
		   "  -h, --help           Display help and exit.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname)
		);
}
