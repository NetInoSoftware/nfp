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

#include "nfp.h"
#include "linux_sigaction.h"
#include "cli_arg_parse.h"
#include "classifier.h"
#include "classifier_term.h"

#if ODP_VERSION_API_GENERATION <= 1 && ODP_VERSION_API_MAJOR < 20
	#define ODP_PMR_INVALID ODP_PMR_INVAL
#endif

#define MAX_WORKERS		32

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
	enum TERMINATE_METHOD method;
} appl_args_t;

typedef struct {
	void *pkt_func;
	odp_schedule_group_t sched_group;
	nfp_channel_t chn;
} thread_arg_t;

/* helper funcs */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args);
static void parse_args_cleanup(appl_args_t *appl_args);
static void print_info(char *progname, appl_args_t *appl_args,
		       odp_cpumask_t *cpumask);
static void usage(char *progname);
static int create_interfaces(appl_arg_ifs_t *itf_param);
static int configure_interface_addresses(appl_arg_ifs_t *itf_param);

static void print_global_stats(void);

uint64_t received_total;
uint64_t received_pkt_total;

static int worker_dispatcher(void *_arg)
{
	odp_event_t ev;
	odp_packet_t pkt;
	odp_queue_t in_queue;
	int event_idx = 0;
	int event_cnt = 0;
	thread_arg_t arg = *(thread_arg_t *)_arg;
	nfp_pkt_processing_func pkt_func =
		(nfp_pkt_processing_func)arg.pkt_func;
	odp_bool_t *is_running = NULL;
	uint64_t wait_time = 0;
	nfp_param_t nfp_params = {0};
	int rx_burst = 1;
	odp_thrmask_t mask;
	int thr;

	/* Join thread to a scheduler group */
	thr = odp_thread_id();
	odp_thrmask_zero(&mask);
	odp_thrmask_set(&mask, thr);
	if (odp_schedule_group_join(arg.sched_group, &mask) < 0) {
		NFP_ERR("odp_schedule_group_join failed");
		return -1;
	}

	if (nfp_get_parameters(&nfp_params)) {
		NFP_ERR("nfp_get_parameters failed");
		return -1;
	}

	rx_burst = nfp_params.global_param.evt_rx_burst_size;
	odp_event_t events[rx_burst];

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	wait_time = odp_schedule_wait_time(100 * ODP_TIME_MSEC_IN_NS);

	/* PER CORE DISPATCHER */
	while (*is_running) {
		event_cnt = odp_schedule_multi(&in_queue, wait_time,
					       events, rx_burst);
		if (!event_cnt)
			continue;

		for (event_idx = 0; event_idx < event_cnt; event_idx++) {
			odp_event_type_t ev_type;
			odp_event_subtype_t ev_subtype;

			ev = events[event_idx];

			if (ev == ODP_EVENT_INVALID)
				continue;

			ev_type = odp_event_types(ev, &ev_subtype);

			if (odp_likely(ev_type == ODP_EVENT_PACKET)) {
				pkt = odp_packet_from_event(ev);

				nfp_packet_input(pkt, in_queue, pkt_func);
				continue;
			}

			if (ev_type == ODP_EVENT_TIMEOUT) {
				nfp_timer_handle(ev);
				continue;
			}

			NFP_ERR("Unexpected event type: %u", ev_type);
			odp_event_free(ev);
		}
		nfp_send_pending_pkt();
	}

	return 0;
}

static int worker_classif_dispatcher(void *_arg)
{
	odp_event_t ev;
	odp_packet_t pkt;
	odp_queue_t in_queue;
	int event_idx = 0;
	int event_cnt = 0;
	thread_arg_t arg = *(thread_arg_t *)_arg;
	nfp_pkt_processing_multi_func pkt_func =
		(nfp_pkt_processing_multi_func)arg.pkt_func;
	nfp_channel_t chn = arg.chn;
	odp_bool_t *is_running = NULL;
	uint64_t wait_time = 0;
	nfp_param_t nfp_params = {0};
	int rx_burst = 1;
	odp_thrmask_t mask;
	int thr;
	nfp_pkt_vector_t vec;

	/* Join thread to a scheduler group */
	thr = odp_thread_id();
	odp_thrmask_zero(&mask);
	odp_thrmask_set(&mask, thr);
	if (odp_schedule_group_join(arg.sched_group, &mask) < 0) {
		NFP_ERR("odp_schedule_group_join failed");
		return -1;
	}

	if (nfp_get_parameters(&nfp_params)) {
		NFP_ERR("nfp_get_parameters failed");
		return -1;
	}

	rx_burst = nfp_params.global_param.evt_rx_burst_size;
	odp_event_t events[rx_burst];

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	wait_time = odp_schedule_wait_time(100 * ODP_TIME_MSEC_IN_NS);

	nfp_pkt_vector_init(&vec);

	/* PER CORE DISPATCHER */
	while (*is_running) {
		event_cnt = odp_schedule_multi(&in_queue, wait_time,
					       events, rx_burst);
		if (!event_cnt)
			continue;

		for (event_idx = 0; event_idx < event_cnt; event_idx++) {
			odp_event_type_t ev_type;
			odp_event_subtype_t ev_subtype;

			ev = events[event_idx];

			if (ev == ODP_EVENT_INVALID)
				continue;

			ev_type = odp_event_types(ev, &ev_subtype);

			if (odp_likely(ev_type == ODP_EVENT_PACKET)) {
				pkt = odp_packet_from_event(ev);

				if (nfp_pkt_vector_add(&vec, pkt) != 1) {
					odp_packet_free(pkt);
					continue;
				}

				if (!NFP_PKT_VECTOR_IS_FULL(&vec))
					continue;

				if (chn == NFP_CHANNEL_INVALID)
					nfp_packet_input_multi(&vec, in_queue,
							       pkt_func);
				else
					channel_packet_input(&vec, chn);

				nfp_pkt_vector_init(&vec);
				continue;
			}

			if (ev_type == ODP_EVENT_TIMEOUT) {
				nfp_timer_handle(ev);
				continue;
			}

			NFP_ERR("Unexpected event type: %u", ev_type);
			odp_event_free(ev);
		}
		if (NFP_PKT_VECTOR_HAS_DATA(&vec)) {
			if (chn == NFP_CHANNEL_INVALID)
				nfp_packet_input_multi(&vec, in_queue,
						       pkt_func);
			else
				channel_packet_input(&vec, chn);

			nfp_pkt_vector_init(&vec);
		}
		nfp_send_pending_pkt();
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
	nfp_thread_t thread_cls, thread_term;
	nfp_thread_param_t thread_param;
	thread_arg_t thread_arg, thread_arg_cls;
	int core_count, num_workers, ret_val, i;
	odp_cpumask_t cpumask_workers, cpumask_cls, cpumask_term;
	odp_thrmask_t thrmask;
	odp_schedule_group_t group_workers = ODP_SCHED_GROUP_INVALID;
	odp_schedule_group_t group_cls = ODP_SCHED_GROUP_INVALID;
	int retcode = EXIT_SUCCESS;
	classif_t classif;
	int sd = -1;
	nfp_channel_t chn = NFP_CHANNEL_INVALID;

	init_classifier(&classif);

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse and store the application arguments */
	if (parse_args(argc, argv, &params) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* Initialize NFP instance */
	nfp_initialize_param(&app_init_params);
	app_init_params.cli.os_thread.start_on_init = 1;
	app_init_params.linux_core_id = 0;
	app_init_params.enable.cls = 1;
	app_init_params.enable.channel = 1;

	if (nfp_initialize(&app_init_params)) {
		NFP_ERR("Error: NFP global init failed.\n");
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/*
	 * Processing vs cores allocation:
	 * Cores #0 and #1 run background tasks and control threads.
	 * Core  #1 terminates traffic with posix or channel API
	 * Core  #2 receives classified traffic.
	 * Cores #3 runs 'generic' processing threads.
	 */
	core_count = odp_cpu_count();
	if (core_count < 4) {
		NFP_ERR("Error: Invalid number of cores for this "
			"application (min 4).");
		retcode = EXIT_FAILURE;
		goto end;
	}

	num_workers = core_count - 3;
	if (params.core_count && params.core_count < num_workers)
		num_workers = params.core_count;

	odp_cpumask_zero(&cpumask_workers);
	for (i = 0; i < num_workers; i++)
		odp_cpumask_set(&cpumask_workers, 3 + i);

	odp_cpumask_zero(&cpumask_cls);
	odp_cpumask_set(&cpumask_cls, 2);

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params, &cpumask_workers);

	/* Create the scheduler goups */
	odp_thrmask_zero(&thrmask);
	group_workers = odp_schedule_group_create("sched_grp_workers",
						  &thrmask);
	if (group_workers == ODP_SCHED_GROUP_INVALID) {
		NFP_ERR("Error: Failed to create the workers sched group");
		retcode = EXIT_FAILURE;
		goto end;
	}

	odp_thrmask_zero(&thrmask);
	group_cls = odp_schedule_group_create("sched_grp_cls", &thrmask);
	if (group_cls == ODP_SCHED_GROUP_INVALID) {
		NFP_ERR("Error: Failed to create the workers sched group");
		retcode = EXIT_FAILURE;
		goto end;
	}

	/* Create the interfaces with classifier_enable */
	if (create_interfaces(&params.itf_param) < 0) {
		NFP_ERR("Failed to init interface %s",
			params.itf_param.if_array[i].if_name);
		retcode = EXIT_FAILURE;
		goto end;
	}

	/* Build classifier */
	if (build_classifier(&params.itf_param, group_cls, group_workers,
			     &classif)) {
		NFP_ERR("Error: Failed to build the classifier.\n");
		retcode = EXIT_FAILURE;
		goto end;
	}

	if (params.method == METHOD_CHANNEL) {
		chn = init_channel();
		if (chn == NFP_CHANNEL_INVALID) {
			NFP_ERR("Error: Failed to create the channel ");
			nfp_stop_processing();
			nfp_thread_join(thread_tbl, num_workers);
			retcode = EXIT_FAILURE;
			goto end;
		}
	}

	/* Start worker threads */
	memset(thread_tbl, 0, sizeof(thread_tbl));
	nfp_thread_param_init(&thread_param);
	thread_param.start = worker_dispatcher;
	thread_param.arg = &thread_arg;
	thread_param.thr_type = ODP_THREAD_WORKER;
	thread_param.description = "worker";
	thread_arg.pkt_func = nfp_eth_vlan_processing;
	thread_arg.sched_group = group_workers;
	thread_arg.chn = NFP_CHANNEL_INVALID;

	ret_val = nfp_thread_create(thread_tbl,
				    num_workers,
				    &cpumask_workers,
				    &thread_param);
	if (ret_val != num_workers) {
		NFP_ERR("Error: Failed to create worker threads, "
			"expected %d, got %d",
			num_workers, ret_val);
		nfp_stop_processing();
		if (ret_val != -1)
			nfp_thread_join(thread_tbl, ret_val);
		retcode = EXIT_FAILURE;
		goto end;
	}

	/* Start classifier processing thread */
	nfp_thread_param_init(&thread_param);
	thread_param.start = worker_classif_dispatcher;
	thread_param.arg = &thread_arg_cls;
	thread_param.thr_type = ODP_THREAD_WORKER;
	thread_param.description = "classif";
	if (params.method == METHOD_POSIX) {
		thread_arg_cls.pkt_func = nfp_udp4_processing_multi;
		thread_arg_cls.chn = NFP_CHANNEL_INVALID;
	} else {	/* channel termination*/
		thread_arg_cls.pkt_func = NULL;
		thread_arg_cls.chn = chn;
	}
	thread_arg_cls.sched_group = group_cls;

	ret_val = nfp_thread_create(&thread_cls, 1,
				    &cpumask_cls,
				    &thread_param);
	if (ret_val != 1) {
		NFP_ERR("Error: Failed to create classifier thread");
		nfp_stop_processing();
		nfp_thread_join(thread_tbl, num_workers);
		retcode = EXIT_FAILURE;
		goto end;
	}

	/* Configure IP addresses */
	if (configure_interface_addresses(&params.itf_param)) {
		NFP_ERR("Error: Failed to configure addresses");
		retcode = EXIT_FAILURE;
		goto join_workers;
	}

	/*
	 * Process the CLI commands file (if defined).
	 * This is an alternative way to set the IP addresses and other
	 * parameters.
	 */
	if (nfp_cli_process_file(params.cli_file)) {
		NFP_ERR("Error: Failed to process CLI file.");
		retcode = EXIT_FAILURE;
		goto join_workers;
	}

	if (params.method == METHOD_POSIX) {
		sd = init_posix(TEST_PORT);
		if (sd == -1) {
			NFP_ERR("Error: Failed to init_posix");
			retcode = EXIT_FAILURE;
			goto join_workers;
		}
	}

	/* Start termination thread on core #1 */
	nfp_thread_param_init(&thread_param);
	thread_param.thr_type = ODP_THREAD_CONTROL;
	thread_param.description = "termination";
	if (params.method == METHOD_POSIX) {
		thread_param.start = termination_posix;
		thread_param.arg = &sd;
	} else {
		thread_param.start = termination_channel;
		thread_param.arg = &chn;
	}
	odp_cpumask_zero(&cpumask_term);
	odp_cpumask_set(&cpumask_term, 1);

	ret_val = nfp_thread_create(&thread_term, 1,
				    &cpumask_term,
				    &thread_param);
	if (ret_val != 1) {
		NFP_ERR("Error: Failed to create termination thread");
		retcode = EXIT_FAILURE;
		goto join_workers;
	}

	/* Print statistics (in a loop) */
	print_global_stats();

	/* Cleanup */
	nfp_thread_join(&thread_term, 1);

join_workers:
	cleanup_posix(sd);
	cleanup_channel(chn);

	nfp_stop_processing();
	nfp_thread_join(thread_tbl, num_workers);
	nfp_thread_join(&thread_cls, 1);

end:
	cleanup_classifier(&classif);

	if (group_workers != ODP_SCHED_GROUP_INVALID) {
		odp_schedule_group_destroy(group_workers);
		group_workers = ODP_SCHED_GROUP_INVALID;
	}

	if (group_cls != ODP_SCHED_GROUP_INVALID) {
		odp_schedule_group_destroy(group_cls);
		group_cls = ODP_SCHED_GROUP_INVALID;
	}

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed.\n");

	parse_args_cleanup(&params);
	printf("End Main()\n");
	return retcode;
}

/**
 * Parse and store the command line arguments
 *
 * @param argc       argument count
 * @param argv[]     argument vector
 * @param appl_args  Store application arguments here
 * @return int EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args)
{
	int opt, res = 0;
	int long_index;
	size_t len;
	static struct option longopts[] = {
		{"count", required_argument, NULL, 'c'},
		{"interface", required_argument, NULL, 'i'},
		{"help", no_argument, NULL, 'h'},
		{"cli-file", required_argument,	NULL, 'f'},
		{"method", required_argument, NULL, 'm'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->method = METHOD_CHANNEL;

	while (res == 0) {
		opt = getopt_long(argc, argv, "+c:i:hf:m:",
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
 * Cleanup cli parameters
 */
static void parse_args_cleanup(appl_args_t *appl_args)
{
	nfpexpl_parse_interfaces_param_cleanup(&appl_args->itf_param);
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
		   progname, appl_args->itf_param.if_count);
	for (i = 0; i < appl_args->itf_param.if_count; ++i)
		printf(" %s", appl_args->itf_param.if_array[i].if_name);
	printf("\n\n");

	/* Print worker to core distribution */
	if (odp_cpumask_to_str(cpumask, cpumaskstr, sizeof(cpumaskstr)) < 0) {
		printf("Error: Too small buffer provided to "
			"odp_cpumask_to_str\n");
		strcpy(cpumaskstr, "Unknown");
	}

	printf("Num worker threads: %i\n", odp_cpumask_count(cpumask));
	printf("first CPU:          %i\n", odp_cpumask_first(cpumask));
	printf("cpu mask:           %s\n", cpumaskstr);

	printf("Termination method: %s\n", appl_args->method == METHOD_CHANNEL ?
	       "channel" : "posix");

	fflush(NULL);
}

/**
 * Prinf usage information
 */
static void usage(char *progname)
{
	printf("\n"
		   "Classifies traffic to terminate certain UDP packets. See Note.\n\n"
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
		   "  -c, --count <number> Core count.\n"
		   "  -f, --cli-file <file> NFP CLI file.\n"
		   "  -m, --method <method> Packet termination method\n"
		   "  where <method>\n"
		   "    0  - channel API. Default value.\n"
		   "    1  - posix API\n"
		   "  -h, --help           Display help and exit.\n"
		   "\nNote: The UDP packets received on "
		   "<local-address, local-port> are classified and processed "
		   "on a separate core (#3), where:\n"
		   "    - local-address is the IPv4 address from first "
		   "interface (fp0)\n"
		   "    - local-port is %d\n"
		   "Finally, traffic is terminated with 'posix' or 'channel' "
		   "API on core #1 and statistics are displayed.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname), TEST_PORT);
}

static int create_interfaces(appl_arg_ifs_t *itf_param)
{
	int i;
	nfp_ifport_net_param_t if_param;
	odp_pktio_param_t pktio_param;
	odp_pktin_queue_param_t pktin_param;
	odp_queue_param_t *queue_param;

	nfp_ifport_net_param_init(&if_param);
	if_param.pktio_param = &pktio_param;
	if_param.pktin_param = &pktin_param;

	odp_pktio_param_init(&pktio_param);
	pktio_param.in_mode = ODP_PKTIN_MODE_SCHED;
	pktio_param.out_mode = ODP_PKTOUT_MODE_DIRECT;

	odp_pktin_queue_param_init(&pktin_param);
	pktin_param.num_queues = 1;
	pktin_param.classifier_enable = 1;

	queue_param = &pktin_param.queue_param;
	odp_queue_param_init(queue_param);
	queue_param->type = ODP_QUEUE_TYPE_SCHED;
	queue_param->enq_mode = ODP_QUEUE_OP_MT;
	queue_param->deq_mode = ODP_QUEUE_OP_MT;
	queue_param->context = NULL;
	queue_param->sched.prio = odp_schedule_default_prio();
	queue_param->sched.sync = ODP_SCHED_SYNC_ATOMIC;
	queue_param->sched.group = ODP_SCHED_GROUP_ALL;

	for (i = 0; i < itf_param->if_count && i < NFP_FP_INTERFACE_MAX; i++)
		if (nfp_ifport_net_create(itf_param->if_array[i].if_name,
					  &if_param, NULL, NULL) < 0) {
			NFP_ERR("Failed to init interface %s",
				itf_param->if_array[i].if_name);
			return -1;
		}

	return 0;
}

/** Configure IPv4 addresses
 *
 * @param itf_param appl_arg_ifs_t Interfaces to configure
 * @return int 0 on success, -1 on error
 *
 */
static int configure_interface_addresses(appl_arg_ifs_t *itf_param)
{
	struct appl_arg_if *ifarg = NULL;
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	uint32_t addr = 0;
	int port = 0;
	uint16_t subport = 0;
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

		ifnet = nfp_ifport_net_ifnet_get_by_name(ifarg->if_name);
		if (ifnet == NFP_IFNET_INVALID) {
			NFP_ERR("Error: interface not found: %s",
				ifarg->if_name);
			ret = -1;
			break;
		}

		if (nfp_ifnet_port_get(ifnet, &port, &subport)) {
			NFP_ERR("Error: Failed to get <port, sub-port>: %s",
				ifarg->if_name);
			ret = -1;
			break;
		}

		if (!nfp_parse_ip_addr(ifarg->if_address, &addr)) {
			NFP_ERR("Error: Failed to parse IPv4 address: %s",
				ifarg->if_address);
			ret = -1;
			break;
		}

		res = nfp_ifport_net_ipv4_up(port, subport, 0, addr,
					     ifarg->if_address_masklen, 1);
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
