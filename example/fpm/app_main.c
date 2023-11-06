/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>

#include "nfp.h"
#include "linux_sigaction.h"
#include "cli_arg_parse.h"

#define MAX_WORKERS		32

/**
 * Get rid of path in filename - only for unix-type paths using '/'
 */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

/**
 * Parsed command line application arguments
 */
typedef struct {
	int core_count;
	appl_arg_ifs_t itf_param;
	char *cli_file;
	int perf_stat;
	odp_bool_t single_pkt_API;
} appl_args_t;

/**
 * helper funcs
 */
static int parse_args(int argc, char *argv[], appl_args_t *appl_args);
static void parse_args_cleanup(appl_args_t *appl_args);
static int configure_interface_addresses(appl_arg_ifs_t *itf_param);
static void print_info(char *progname, appl_args_t *appl_args,
		       odp_cpumask_t *cpumask);
static void usage(char *progname);
static int start_performance(nfp_thread_t *thread_perf, int core_id);

/**
 * main() Application entry point
 *
 * This is the main function of the FPM application, it's a minimalistic
 * example, see 'usage' function for available arguments and usage.
 *
 * Using the number of available cores as input, this example sets up
 * ODP dispatcher threads executing NFP VLAN processesing and starts
 * a CLI function on a managment core.
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
	nfp_thread_t thread_perf;

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse and store the application arguments */
	if (parse_args(argc, argv, &params) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/*
	 * This example assumes that core #0 and #1 runs Linux kernel
	 * background tasks and control threads.
	 * By default, cores #2 and beyond will be populated with a NFP
	 * processing threads each.
	 */
	nfp_initialize_param(&app_init_params);
	app_init_params.evt_rx_burst_size = NFP_PKT_VECTOR_SIZE;
	app_init_params.pkt_tx_burst_size = NFP_PKT_VECTOR_SIZE;
	app_init_params.cli.os_thread.start_on_init = 1;

	if (app_init_params.pktin_mode != ODP_PKTIN_MODE_SCHED) {
		printf("Warning: Forcing scheduled pktin mode.\n");
		/* Receive packets through the ODP scheduler. */
		app_init_params.pktin_mode = ODP_PKTIN_MODE_SCHED;
	}

	app_init_params.if_count = params.itf_param.if_count;
	for (i = 0; i < params.itf_param.if_count && i < NFP_FP_INTERFACE_MAX; i++) {
		strncpy(app_init_params.if_names[i],
			params.itf_param.if_array[i].if_name,
			NFP_IFNAMSIZ);
		app_init_params.if_names[i][NFP_IFNAMSIZ - 1] = '\0';
	}

	/*
	 * Initialize NFP. This will also initialize ODP and open a pktio
	 * instance for each interface supplied as argument.
	 */
	if (nfp_initialize(&app_init_params) != 0) {
		printf("Error: NFP global init failed.\n");
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/*
	 * Get the default workers-to-cores distribution: one
	 * run-to-completion worker thread or process can be created per core.
	 */
	if (nfp_get_default_worker_cpumask(params.core_count, MAX_WORKERS,
					   &cpumask_workers)) {
		NFP_ERR("Error: Failed to get the default workers to cores "
			"distribution\n");
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}
	num_workers = odp_cpumask_count(&cpumask_workers);

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params, &cpumask_workers);

	/*
	 * Create and launch dataplane dispatcher worker threads to be placed
	 * according to the cpumask, thread_tbl will be populated with the
	 * created pthread IDs.
	 *
	 * In this case, all threads will run the default_event_dispatcher
	 * function with nfp_eth_vlan_processing as argument.
	 *
	 * If different dispatchers should run, or the same be run with differnt
	 * input arguments, the cpumask is used to control this.
	 */

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
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/* Configure IP addresses */
	if (configure_interface_addresses(&params.itf_param)) {
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

	/*
	 * If we choose to check performance, a performance monitoring thread
	 * will be started on the management core. Once every second it will
	 * read the statistics from the workers from a shared memory region.
	 * Using this has negligible performance impact (<<0.01%).
	 */
	if (params.perf_stat) {
		if (start_performance(&thread_perf,
				      app_init_params.linux_core_id) <= 0) {
			NFP_ERR("Error: Failed to init performance monitor");
			nfp_stop_processing();
			nfp_thread_join(thread_tbl, num_workers);
			nfp_terminate();
			parse_args_cleanup(&params);
			return EXIT_FAILURE;
		}
	}

	/*
	 * Wait here until all worker threads have terminated, then free up all
	 * resources allocated by odp_init_global().
	 */
	nfp_thread_join(thread_tbl, num_workers);
	if (params.perf_stat)
		nfp_thread_join(&thread_perf, 1);

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed\n");

	parse_args_cleanup(&params);
	printf("FPM End Main()\n");
	return EXIT_SUCCESS;
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
		{"performance", no_argument, NULL, 'p'},	/* return 'p' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"single-pkt-API", no_argument, NULL, 'g'},
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->single_pkt_API = 0;

	while (1) {
		opt = getopt_long(argc, argv, "+c:i:hpf:g",
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
				parse_args_cleanup(appl_args);
				return EXIT_FAILURE;
			}
			break;

		case 'h':
			usage(argv[0]);
			parse_args_cleanup(appl_args);
			return EXIT_FAILURE;

		case 'p':
			appl_args->perf_stat = 1;
			break;

		case 'f':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				parse_args_cleanup(appl_args);
				return EXIT_FAILURE;
			}
			len += 1;	/* add room for '\0' */

			appl_args->cli_file = malloc(len);
			if (appl_args->cli_file == NULL) {
				usage(argv[0]);
				parse_args_cleanup(appl_args);
				return EXIT_FAILURE;
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

	if (appl_args->itf_param.if_count == 0) {
		usage(argv[0]);
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

	printf("Packet processing API: %s\n\n", appl_args->single_pkt_API ?
	       "single" : "multi");

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
		   "  -i, --interface <interfaces> Ethernet interface list"
		   " (comma-separated, no spaces)\n"
		   "  Example:\n"
		   "    eth1,eth2\n"
		   "    eth1@192.168.100.10/24,eth2@172.24.200.10/16\n"
		   "\n"
		   "Optional OPTIONS\n"
		   "  -c, --count <number>  Core count.\n"
		   "  -p, --performance     Performance Statistics.\n"
		   "  -f, --cli-file <file> NFP CLI file.\n"
		   "  -g, --single-pkt-API  Use single packet processing API\n"
		   "  -h, --help            Display help and exit.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname)
		);
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

static int perf_client(void *arg)
{
	odp_bool_t *is_running = NULL;
	struct nfp_perf_stat *ps = NULL;
	(void) arg;

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	nfp_set_stat_flags(NFP_STAT_COMPUTE_PERF);

	while (*is_running) {
		ps = nfp_get_perf_statistics();
		printf ("Mpps:%4.3f\n", ((float)ps->rx_fp_pps)/1000000);
		usleep(1000000UL);
	}

	return 0;
}

static int start_performance(nfp_thread_t *thread_perf, int core_id)
{
	odp_cpumask_t cpumask;
	nfp_thread_param_t thread_param = {0};

	odp_cpumask_zero(&cpumask);
	odp_cpumask_set(&cpumask, core_id);

	nfp_thread_param_init(&thread_param);
	thread_param.start = perf_client;
	thread_param.arg = NULL;
	thread_param.thr_type = ODP_THREAD_CONTROL;
	thread_param.description = "perf";

	return nfp_thread_create(thread_perf,
					 1,
					 &cpumask,
					 &thread_param);
}
