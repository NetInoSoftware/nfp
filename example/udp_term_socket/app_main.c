/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <stdio.h>

#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <inttypes.h>

#include "nfp.h"
#include "udp_term_socket.h"
#include "linux_sigaction.h"
#include "cli_arg_parse.h"

#define MAX_WORKERS		64

#define PKT_BURST_SIZE NFP_PKT_VECTOR_SIZE

/** Get rid of path in filename - only for unix-type paths using '/' */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

enum TERMINATE_METHOD {
	METHOD_CHANNEL = 0,
	METHOD_POSIX,
	METHOD_MAX
};

/**
 * Parsed command line application arguments
 */
typedef struct {
	int core_count;
	appl_arg_ifs_t itf_param;
	char *cli_file;
	char *laddr;
	uint16_t lport;
	enum TERMINATE_METHOD method;
	odp_bool_t single_pkt_API;
} appl_args_t;

struct pktio_thr_arg {
	int num_pktin;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
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

static int pkt_io_recv_single(void *arg)
{
	odp_packet_t pkt, pkt_tbl[PKT_BURST_SIZE];
	int pkt_idx, pkt_cnt;
	struct pktio_thr_arg *thr_args;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t *is_running = NULL;

	thr_args = arg;
	num_pktin = thr_args->num_pktin;

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

	return 0;
}

static int pkt_io_recv(void *arg)
{
	odp_packet_t pkt, pkt_tbl[PKT_BURST_SIZE];
	int pkt_idx, pkt_cnt;
	nfp_pkt_vector_t vec;
	struct pktio_thr_arg *thr_args;
	int num_pktin, i;
	odp_pktin_queue_t pktin[NFP_FP_INTERFACE_MAX];
	uint8_t *ptr;
	odp_bool_t *is_running = NULL;

	thr_args = arg;
	num_pktin = thr_args->num_pktin;

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

	nfp_pkt_vector_init(&vec);

	while (*is_running) {
		for (i = 0; i < num_pktin; i++) {
			pkt_cnt = odp_pktin_recv(pktin[i], pkt_tbl,
						 PKT_BURST_SIZE);

			for (pkt_idx = 0; pkt_idx < pkt_cnt; pkt_idx++) {
				pkt = pkt_tbl[pkt_idx];

				if (nfp_pkt_vector_add(&vec, pkt) != 1) {
					odp_packet_free(pkt);
					continue;
				}

				if (!NFP_PKT_VECTOR_IS_FULL(&vec))
					continue;

				nfp_packet_input_multi(&vec, ODP_QUEUE_INVALID,
						       nfp_eth_vlan_processing_multi);
				nfp_pkt_vector_init(&vec);
			}
		}

		if (NFP_PKT_VECTOR_HAS_DATA(&vec)) {
			nfp_packet_input_multi(&vec, ODP_QUEUE_INVALID,
					       nfp_eth_vlan_processing_multi);
			nfp_pkt_vector_init(&vec);
		}
		nfp_send_pending_pkt();
	}

	return 0;
}

/*
 * Should receive timeouts only
 */
static int event_dispatcher(void *arg)
{
	odp_queue_t in_queue;
	odp_event_t ev;
	odp_bool_t *is_running = NULL;

	(void)arg;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	while (*is_running) {
		ev = odp_schedule(&in_queue, ODP_SCHED_WAIT);

		if (ev == ODP_EVENT_INVALID)
			continue;

		if (odp_event_type(ev) == ODP_EVENT_TIMEOUT) {
			nfp_timer_handle(ev);
			continue;
		} else if (odp_event_type(ev) == ODP_EVENT_PACKET) {
			/* loopbacked, SP pkts */
			nfp_packet_input(odp_packet_from_event(ev),
					 in_queue,
					 nfp_eth_vlan_processing);
			nfp_send_pending_pkt();
			continue;
		}

		NFP_ERR("Error: unexpected event type: %u\n",
			odp_event_type(ev));

		odp_event_free(ev);
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

static void print_global_stats(void)
{
	odp_bool_t *is_running = NULL;
	int idx = 0;
	uint64_t last_received_total = 0;
	uint64_t last_received_pkt_total = 0;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return;
	}

	while (*is_running) {
		sleep(1);
		if (++idx < 10)
			continue;

		printf("Received bytes/sec: %" PRIu64 ", "
		       "pkt/sec: %" PRIu64 "\n",
		       (received_total - last_received_total) / 10,
		       (received_pkt_total - last_received_pkt_total) / 10);
		idx = 0;
		last_received_total = received_total;
		last_received_pkt_total = received_pkt_total;
	}
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
	nfp_thread_t thread_tbl[MAX_WORKERS], dispatcher_thread;
	nfp_thread_t termination_thread;
	nfp_thread_param_t thread_param;
	int num_workers, tx_queues, first_worker, i;
	odp_cpumask_t cpu_mask;
	struct pktio_thr_arg pktio_thr_args[MAX_WORKERS];
	struct interface_id itf_id[NFP_FP_INTERFACE_MAX];
	nfp_ifport_net_param_t if_param;
	odp_pktio_param_t pktio_param;
	odp_pktin_queue_param_t pktin_param;
	odp_pktout_queue_param_t pktout_param;
	odp_pktio_t pktio;
	int sd = -1;
	nfp_channel_t chn = NFP_CHANNEL_INVALID;

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse and store the application arguments */
	if (parse_args(argc, argv, &params) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* Initialize NFP */
	nfp_initialize_param(&app_init_params);
	app_init_params.cli.os_thread.start_on_init = 1;
	app_init_params.linux_core_id = 0;

	if (nfp_initialize(&app_init_params)) {
		NFP_ERR("Error: NFP global init failed.\n");
		parse_args_cleanup(&params);
		exit(EXIT_FAILURE);
	}

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params);

	/*
	 * core #0 runs background tasks and processes timer events.
	 * core #1 runs traffic termination code on a control thread
	 * core #2 - #max run stack worker threads
	 * Note: Core #0 and #1 require TX queues.
	 */
	first_worker = 2;
	num_workers = odp_cpu_count() - 2;

	if (params.core_count && params.core_count < num_workers)
		num_workers = params.core_count;
	if (num_workers > MAX_WORKERS)
		num_workers = MAX_WORKERS;
	tx_queues = num_workers + 2;

	if (num_workers < 1) {
		NFP_ERR("ERROR: At least 3 cores required.\n");
		goto end;
	}

	printf("Num worker threads: %i\n", num_workers);
	printf("First worker CPU:   %i\n\n", first_worker);

	memset(pktio_thr_args, 0, sizeof(pktio_thr_args));

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
	pktin_param.hash_proto.proto.ipv6_udp = 1;
	pktin_param.num_queues = num_workers;

	odp_pktout_queue_param_init(&pktout_param);
	pktout_param.op_mode = ODP_PKTIO_OP_MT; /* see note below */
	pktout_param.num_queues = tx_queues;

	/** Note: Using output queues as multithread unsafe
	 * (ODP_PKTIO_OP_MT_UNSAFE) is tricky and should be avoided.
	 */

	for (i = 0; i < params.itf_param.if_count; i++) {
		int j;
		odp_pktin_queue_t pktin[num_workers];
		struct appl_arg_if *if_arg = &params.itf_param.if_array[i];

		if (nfp_ifport_net_create(if_arg->if_name, &if_param,
					  &itf_id[i].port,
					  &itf_id[i].subport) < 0) {
			NFP_ERR("Failed to init interface %s", if_arg->if_name);
			goto end;
		}

		pktio = odp_pktio_lookup(if_arg->if_name);
		if (pktio == ODP_PKTIO_INVALID) {
			NFP_ERR("Failed locate pktio %s", if_arg->if_name);
			goto end;
		}

		if (odp_pktin_queue(pktio, pktin, num_workers) != num_workers) {
			NFP_ERR("Too few pktin queues for %s", if_arg->if_name);
			goto end;
		}

		if (odp_pktout_queue(pktio, NULL, 0) != tx_queues) {
			NFP_ERR("Too few pktout queues for %s",
				if_arg->if_name);
			goto end;
		}

		for (j = 0; j < num_workers; j++)
			pktio_thr_args[j].pktin[i] = pktin[j];
	}

	memset(thread_tbl, 0, sizeof(thread_tbl));

	for (i = 0; i < num_workers; ++i) {
		pktio_thr_args[i].num_pktin = params.itf_param.if_count;

		nfp_thread_param_init(&thread_param);
		if (params.single_pkt_API)
			thread_param.start = pkt_io_recv_single;
		else
			thread_param.start = pkt_io_recv;
		thread_param.arg = &pktio_thr_args[i];
		thread_param.thr_type = ODP_THREAD_WORKER;
		thread_param.description = "worker";

		odp_cpumask_zero(&cpu_mask);
		odp_cpumask_set(&cpu_mask, first_worker + i);

		if (nfp_thread_create(&thread_tbl[i], 1,
				      &cpu_mask, &thread_param) != 1)
			break;
	}

	if (i < num_workers) {
		NFP_ERR("Error: Failed to create worker threads, "
			"expected %d, got %d", num_workers, i);
		nfp_stop_processing();
		if (i > 0)
			nfp_thread_join(thread_tbl, i);
		goto end;
	}

	/* Start timer events processing thread */
	odp_cpumask_zero(&cpu_mask);
	odp_cpumask_set(&cpu_mask, app_init_params.linux_core_id);
	nfp_thread_param_init(&thread_param);
	thread_param.start = event_dispatcher;
	thread_param.arg = NULL;
	thread_param.thr_type = ODP_THREAD_WORKER;
	thread_param.description = "timer_events";
	if (nfp_thread_create(&dispatcher_thread, 1,
			      &cpu_mask, &thread_param) != 1) {
		NFP_ERR("Error: Failed to create dispatcherthreads");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		goto end;
	}

	/* Configure IP addresses */
	if (configure_interface_addresses(&params.itf_param, itf_id)) {
		NFP_ERR("Error: Failed to configure addresses");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		nfp_thread_join(&dispatcher_thread, 1);
		goto end;
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
		nfp_thread_join(&dispatcher_thread, 1);
		goto end;
	}
	sleep(1);

	if (params.method == METHOD_CHANNEL) {
		chn = udp_channel_config();
		if (chn == NFP_CHANNEL_INVALID) {
			NFP_ERR("Error: udp_channel_config failed.");
			goto end;
		}
	}

	sd = udp_socket_cfg(params.laddr, params.lport, &chn);
	if (sd == -1) {
		NFP_ERR("Error: udp_socket_cfg failed.");
		nfp_stop_processing();
		nfp_thread_join(&dispatcher_thread, 1);
		nfp_thread_join(thread_tbl, num_workers);
		goto end;
	}

	/* Start traffic termination thread */
	odp_cpumask_zero(&cpu_mask);
	odp_cpumask_set(&cpu_mask, app_init_params.linux_core_id + 1);
	nfp_thread_param_init(&thread_param);
	if (params.method == METHOD_CHANNEL) {
		thread_param.start = termination_channel;
		thread_param.arg = &chn;
	} else {
		thread_param.start = termination_posix;
		thread_param.arg = &sd;
	}
	thread_param.thr_type = ODP_THREAD_CONTROL;
	thread_param.description = "termination";
	if (nfp_thread_create(&termination_thread, 1,
			      &cpu_mask, &thread_param) != 1) {
		NFP_ERR("Error: Failed to create dispatcherthreads");
		nfp_stop_processing();
		nfp_thread_join(&dispatcher_thread, 1);
		nfp_thread_join(thread_tbl, num_workers);
		goto end;
	}

	print_global_stats();

	nfp_thread_join(thread_tbl, num_workers);
	nfp_thread_join(&dispatcher_thread, 1);
	nfp_thread_join(&termination_thread, 1);

end:
	if (udp_socket_cleanup(sd))
		printf("Error: udp_socket_cleanup failed.\n");

	if (udp_channel_cleanup(chn))
		printf("Error: udp_channel_cleanup failed.\n");

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed.\n");

	parse_args_cleanup(&params);
	printf("End Main()\n");
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
		{"count", required_argument, NULL, 'c'},
		{"interface", required_argument, NULL, 'i'},	/* return 'i' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"local-address", required_argument,
			NULL, 'l'},/* return 'l' */
		{"local-port", required_argument, NULL, 'p'},
		{"method", required_argument, NULL, 'm'},
		{"single-pkt-API", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));

	appl_args->lport = TEST_LPORT;
	appl_args->method = METHOD_CHANNEL;
	appl_args->single_pkt_API = 0;

	while (res == 0) {
		opt = getopt_long(argc, argv, "+c:i:hf:l:p:m:g",
				  longopts, &long_index);

		if (opt == -1)
			break;	/* No more options */

		switch (opt) {
		case 'c':
			appl_args->core_count = atoi(optarg);
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
			if (len == 0 || atoi(optarg) < 1) {
				usage(argv[0]);
				res = -1;
				break;
			}
			appl_args->lport = atoi(optarg);
			break;
		case 'm':
			len = strlen(optarg);
			if (len == 0 || atoi(optarg) < 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			appl_args->method = atoi(optarg);
			if (appl_args->method >= METHOD_MAX) {
				usage(argv[0]);
				res = -1;
				break;
			}

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

	if (appl_args->laddr == NULL) {
		printf("Error: Invalid local address (null)\n");
		usage(argv[0]);
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
		   "CPU freq (hz):   %" PRIu64 "\n"
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
	printf("Termination method: %s\n", appl_args->method == METHOD_CHANNEL ?
	       "channel" : "posix");
	printf("Packet processing API: %s\n", appl_args->single_pkt_API ?
	       "single" : "multi");
	printf("\n\n");
	fflush(NULL);
}

/**
 * Prinf usage information
 */
static void usage(char *progname)
{
	printf("\n"
		   "Terminates UDP traffic. See Note.\n\n"
		   "Usage: %s OPTIONS\n"
		   "  E.g. %s -i eth1,eth2,eth3\n"
		   "\n"
		   "Mandatory OPTIONS:\n"
		   "  -i, --interface <interfaces> Ethernet interface list"
		   " (comma-separated, no spaces)\n"
		   "  Example:\n"
		   "    eth1,eth2\n"
		   "    eth1@192.168.100.10/24,eth2@172.24.200.10/16\n"
		   "  -l, --local-address   Local address. See Note.\n"
		   "\n"
		   "Optional OPTIONS\n"
		   "  -f, --cli-file <file>       NFP CLI file.\n"
		   "  -c, --count <number>        Core count.\n"
		   "  -p, --local-port            Local port. Default is %d\n"
		   "  -m, --method <method>       Packet termination method\n"
		   "  where <method>\n"
		   "    0  - channel API. Default value.\n"
		   "    1  - posix API\n"
		   "  -g, --single-pkt-API        Use single packet processing API\n"
		   "  -h, --help                  Display help and exit.\n"
		   "Note: The UDP socket is bound to <local-address, local-port>, "
		   "where:\n"
		   "    - local-address is specified with '-l' option\n"
		   "    - local-port is specified with '-p' option\n"
		   "UDP traffic is terminated and statistics are displayed\n"
		   "\n", NO_PATH(progname), NO_PATH(progname), TEST_LPORT);
}

