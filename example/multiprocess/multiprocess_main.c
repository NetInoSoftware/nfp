/* Copyright (c) 2020, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "nfp.h"
#include "cli_arg_parse.h"
#include "linux_sigaction.h"
#include "odp/helper/linux.h"

#define MAX_WORKERS		64
#define LINUX_CONTROL_CPU 0
#define LPORT_DEFAULT 20000
#define RETRY_MAX 10
#define CNT_UDP 10
#define CNT_TCP 100

/** Get rid of path in filename - only for unix-type paths using '/' */
#define NO_PATH(file_name) (strrchr((file_name), '/') ? \
				strrchr((file_name), '/') + 1 : (file_name))

typedef enum {
	APP_MODE_UDP = 0,
	APP_MODE_TCP
} app_mode;

	/**
 * Parsed command line application arguments
 */
typedef struct {
	int core_count;
	appl_arg_ifs_t itf_param;
	char *cli_file;
	char *laddr;
	int lport;
	app_mode mode;
	uint64_t recv_count;
} appl_args_t;

static int parse_args(int argc, char *argv[], appl_args_t *appl_args);
static void parse_args_cleanup(appl_args_t *appl_args);
static int configure_interface_addresses(appl_arg_ifs_t *itf_param);
static void print_info(char *progname, appl_args_t *appl_args);
static void usage(char *progname);

int udp_test(appl_args_t *arg);
int tcp_test(appl_args_t *arg);

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
	nfp_process_t proc_tbl[MAX_WORKERS];
	nfp_process_param_t proc_param = {0};
	int num_workers, i, ret = 0;
	odp_cpumask_t cpumask_workers;
	char cpumaskstr[64];

	/* add handler for Ctr+C */
	if (nfpexpl_sigaction_set(nfpexpl_sigfunction_stop)) {
		printf("Error: failed to set signal actions.\n");
		return EXIT_FAILURE;
	}

	/* Parse and store the application arguments */
	if (parse_args(argc, argv, &params) !=  EXIT_SUCCESS)
		return EXIT_FAILURE;

	/*
	 * This example assumes that core LINUX_CONTROL_CPU runs Linux kernel
	 * background tasks, ODP/NFP management threads and the control process.
	 * By default, cores LINUX_CONTROL_CPU + 1 and beyond will be populated
	 * with a NFP processing workers.
	 */

	nfp_initialize_param(&app_init_params);
	app_init_params.linux_core_id = LINUX_CONTROL_CPU;
	app_init_params.cli.os_thread.start_on_init = 1;
	app_init_params.if_count = params.itf_param.if_count;
	for (i = 0; i < params.itf_param.if_count &&
	     i < NFP_FP_INTERFACE_MAX; i++) {
		strncpy(app_init_params.if_names[i],
			params.itf_param.if_array[i].if_name,
			NFP_IFNAMSIZ);
		app_init_params.if_names[i][NFP_IFNAMSIZ - 1] = '\0';
	}

	/*
	 * Initialize NFP. This will open a pktio instance for each interface
	 * supplied as argument by the user.
	 */

	if (nfp_initialize(&app_init_params) != 0) {
		printf("Error: NFP global init failed.\n");
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	/* Print both system and application information */
	print_info(NO_PATH(argv[0]), &params);

	num_workers = odp_cpu_count() - 1;
	if (params.core_count) {
		num_workers = params.core_count;
		if (num_workers > odp_cpu_count() - 1)
			num_workers = odp_cpu_count() - 1;
	}

	if (num_workers > MAX_WORKERS)
		num_workers = MAX_WORKERS;

	odp_cpumask_zero(&cpumask_workers);
	for (i = 0; i < num_workers; i++)
		odp_cpumask_set(&cpumask_workers, LINUX_CONTROL_CPU + 1 + i);
	if (odp_cpumask_to_str(&cpumask_workers, cpumaskstr,
			       sizeof(cpumaskstr)) < 0) {
		printf("Error: Too small buffer provided to "
		       "odp_cpumask_to_str\n");
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	printf("Control CPU:    %i\n", LINUX_CONTROL_CPU);
	printf("First workers:  %i\n", odp_cpumask_first(&cpumask_workers));
	printf("Num workers:    %i\n", num_workers);
	printf("Workers CPU mask:       %s\n", cpumaskstr);

	/* Start worker processes */
	memset(proc_tbl, 0, sizeof(proc_tbl));
	nfp_process_param_init(&proc_param);
	proc_param.thr_type = ODP_THREAD_WORKER;

	ret = nfp_process_fork_n(proc_tbl, &cpumask_workers, &proc_param);
	if (ret == -1) {
		printf("Error: Failed to start children processes.\n");
		nfp_stop_processing();
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	if (ret == 0) {
		default_event_dispatcher(nfp_eth_vlan_processing);
		parse_args_cleanup(&params);
		odp_term_local();
		exit(0);
	}

	/* Configure IP addresses */
	if (configure_interface_addresses(&params.itf_param)) {
		NFP_ERR("Error: Failed to configure addresses");
		nfp_stop_processing();
		nfp_process_wait_n(proc_tbl, num_workers);
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
		NFP_ERR("Error: Failed to process CLI file");
		nfp_stop_processing();
		nfp_process_wait_n(proc_tbl, num_workers);
		nfp_terminate();
		parse_args_cleanup(&params);
		return EXIT_FAILURE;
	}

	if (params.mode == APP_MODE_UDP)
		udp_test(&params);
	else
		tcp_test(&params);

	nfp_stop_processing();

	nfp_process_wait_n(proc_tbl, num_workers);

	if (nfp_terminate() < 0)
		printf("Error: nfp_terminate failed\n");

	parse_args_cleanup(&params);
	printf("End Main().\n");
	return 0;
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
		{"interface", required_argument, NULL, 'i'},	/* return 'i' */
		{"help", no_argument, NULL, 'h'},		/* return 'h' */
		{"cli-file", required_argument,
			NULL, 'f'},/* return 'f' */
		{"laddr", required_argument,
			NULL, 'l'},/* return 'l' */
		{"lport", required_argument,
			NULL, 'p'}, /*return 'p' */
		{"rcv_count", required_argument,
			NULL, 'n'}, /*return 'n' */
		{"mode", required_argument,	NULL, 'm'}, /* return 'm'*/
		{NULL, 0, NULL, 0}
	};

	memset(appl_args, 0, sizeof(*appl_args));
	appl_args->mode = APP_MODE_UDP;
	appl_args->lport = LPORT_DEFAULT;
	appl_args->recv_count = 0;

	while (res == 0) {
		opt = getopt_long(argc, argv, "+c:i:hf:l:p:m:n:",
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
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			appl_args->lport = atoi(optarg);
			break;
		case 'n':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			appl_args->recv_count = (uint64_t)atoll(optarg);
			break;
		case 'm':
			len = strlen(optarg);
			if (len == 0) {
				usage(argv[0]);
				res = -1;
				break;
			}
			if (!strcmp("u", optarg)) {
				appl_args->mode = APP_MODE_UDP;
			} else if (!strcmp("t", optarg)) {
				appl_args->mode = APP_MODE_TCP;
			} else {
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
		NFP_ERR("Error: Invalid number of interfaces: maximum %d\n",
			NFP_FP_INTERFACE_MAX);
		parse_args_cleanup(appl_args);
		return EXIT_FAILURE;
	}

	if (!appl_args->recv_count) {
		if (appl_args->mode == APP_MODE_UDP)
			appl_args->recv_count = CNT_UDP;
		else
			appl_args->recv_count = CNT_TCP;
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
 * Prinf usage information
 */
static void usage(char *progname)
{
	printf("\n"
		   "Receives UDP/TCP packets in a worker-per-proccess scenario.\n\n"
		   "Usage: %s OPTIONS\n"
		   "  E.g. %s -i eth1,eth2,eth3\n"
		   "\n"
		   "Mandatory OPTIONS:\n"
		   "  -i, --interface <interfaces> Ethernet interface list"
		   " (comma-separated, no spaces)\n"
		   "  Example:\n"
		   "    eth1,eth2\n"
		   "    eth1@192.168.100.10/24,eth2@172.24.200.10/16\n"
		   "\n"
		   "Optional OPTIONS\n"
		   "  -f, --cli-file <file name> CLI commands file\n"
		   "    CLI commands can be used to configure IP addresses, etc.\n"
		   "  -l, --laddr Local address where sockets are bound\n"
		   "    Default: address of the first interface\n"
		   "  -p, --lport <port> Local port where sockets are bound\n"
		   "    Default: %d\n"
		   "  -m, --mode <u|t> Application mode. Default: UDP test\n"
		   "    Modes:\n"
		   "      u - UDP test\n"
		   "      t - TCP test\n"
		   "  -c, --count <number> Core count.\n"
		   "  -n, --rcv_count <number> Received data before exit.\n"
		   "    Argument represents:\n"
		   "      number of datagrams - UDP test. Default %d\n"
		   "      number of bytes - TCP test. Default: %d\n"
		   "  -h, --help           Display help and exit.\n"
		   "\n", NO_PATH(progname), NO_PATH(progname), LPORT_DEFAULT,
		   CNT_UDP, CNT_TCP);
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
	printf("\n\n");
	fflush(NULL);
}

#define BUFF_SIZE 1500
int udp_test(appl_args_t *arg)
{
	int sd;
	struct nfp_sockaddr_in laddr = {0};
	uint32_t my_ip_addr = 0;
	int ret, retry = 0;
	uint64_t i, cnt = arg->recv_count;
	char buff[BUFF_SIZE];
	nfp_ssize_t dgram_size;

	sd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (sd == -1) {
		NFP_ERR("Error: Failed to create socket: errno = %s!\n",
			nfp_strerror(nfp_errno));
		return -1;
	}

	laddr.sin_family = NFP_AF_INET;
	laddr.sin_port = odp_cpu_to_be_16((uint16_t)arg->lport);
	if (arg->laddr) {
		my_ip_addr = inet_addr(arg->laddr);
	} else {
		nfp_ifnet_t ifnet = NFP_IFNET_INVALID;

		ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
		if (ifnet == NFP_IFNET_INVALID) {
			NFP_ERR("Interface not found.");
			return -1;
		}

		if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
					    &my_ip_addr)) {
			NFP_ERR("Faile to get IP address.");
			return -1;
		}
	}
	laddr.sin_addr.s_addr = my_ip_addr;
	laddr.sin_len = sizeof(laddr);

	/* Bind to local address*/
	retry = 0;
	do {
		ret = nfp_bind(sd, (struct nfp_sockaddr *)&laddr,
			       sizeof(struct nfp_sockaddr));
		if (ret < 0) {
			retry++;
			if (retry >= RETRY_MAX)
				break;
			sleep(1);
		}
	} while (ret < 0);

	if (ret < 0) {
		NFP_ERR("Error: Failed to bind: addr=0x%x, port=%d: errno=%s\n",
			my_ip_addr, arg->lport, nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	for (i = 0; i < cnt; i++) {
		dgram_size = nfp_recv(sd, (void *)buff, BUFF_SIZE, 0);
		if (dgram_size == -1) {
			NFP_ERR("Error: nfp_recv() failed: errno=%s.",
				nfp_strerror(nfp_errno));
			break;
		}
		NFP_INFO("%d bytes received", dgram_size);
	}

	nfp_close(sd);
	return 0;
}

int tcp_test(appl_args_t *arg)
{
	int sd, cd;
	struct nfp_sockaddr_in laddr = {0};
	uint32_t my_ip_addr = 0;
	int ret, retry = 0;
	nfp_ssize_t recv_size;
	uint64_t total_recv_size = 0;
	char buff[BUFF_SIZE];

	sd = nfp_socket(NFP_AF_INET, NFP_SOCK_STREAM, NFP_IPPROTO_TCP);
	if (sd == -1) {
		NFP_ERR("Error: Failed to create socket: errno = %s!\n",
			nfp_strerror(nfp_errno));
		return -1;
	}

	laddr.sin_family = NFP_AF_INET;
	laddr.sin_port = odp_cpu_to_be_16((uint16_t)arg->lport);
	if (arg->laddr) {
		my_ip_addr = inet_addr(arg->laddr);
	} else {
		nfp_ifnet_t ifnet = NFP_IFNET_INVALID;

		ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
		if (ifnet == NFP_IFNET_INVALID) {
			NFP_ERR("Interface not found.");
			return -1;
		}

		if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
					    &my_ip_addr)) {
			NFP_ERR("Faile to get IP address.");
			return -1;
		}
	}
	laddr.sin_addr.s_addr = my_ip_addr;
	laddr.sin_len = sizeof(laddr);

	/* Bind to local address*/
	retry = 0;
	do {
		ret = nfp_bind(sd, (struct nfp_sockaddr *)&laddr,
			       sizeof(struct nfp_sockaddr));
		if (ret < 0) {
			retry++;
			if (retry >= RETRY_MAX)
				break;
			sleep(1);
		}
	} while (ret < 0);

	if (ret < 0) {
		NFP_ERR("Error: Failed to bind: addr=0x%x, port=%d: errno=%s\n",
			my_ip_addr, arg->lport, nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	if (nfp_listen(sd, 1)) {
		NFP_ERR("Error: Failed to listen: errno=%s\n",
			nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	cd = nfp_accept(sd, NULL, NULL);
	if (cd == -1) {
		NFP_ERR("Error: Failed to accept connection: errno=%s\n",
			nfp_strerror(nfp_errno));
		nfp_close(sd);
		return -1;
	}

	do {
		recv_size = nfp_recv(cd, (void *)buff, BUFF_SIZE, 0);
		if (recv_size == -1) {
			NFP_ERR("Error: nfp_recv() failed: errno=%s.",
				nfp_strerror(nfp_errno));
			break;
		} else if (recv_size == 0) {
			break;
		}

		total_recv_size += (uint64_t)recv_size;
		NFP_INFO("%ld/%ld bytes received",
			 total_recv_size, arg->recv_count);
	} while (total_recv_size < arg->recv_count);

	nfp_close(cd);
	nfp_close(sd);
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
