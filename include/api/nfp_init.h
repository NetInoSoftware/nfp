/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

/**
 * @file
 *
 * @brief NFP initialization.
 *
 * NFP requires a global level init for the API library before the
 * other NFP APIs may be called.
 * - nfp_initialize()
 *
 * For a graceful termination the matching termination APIs exit
 * - nfp_terminate()
 */

#ifndef __NFP_INIT_H__
#define __NFP_INIT_H__

#include <odp_api.h>
#include "nfp_hook.h"
#include "nfp_ipsec_init.h"
#include "nfp_log.h"
#include "nfp_config.h"
#include "nfp_dns_common.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#define NFP_ODP_INSTANCE_INVALID ((odp_instance_t)(uintptr_t)(-1))

#define NFP_CLI_ADDR_TXT_SIZE 16	/* IPv4 address*/
#define NFP_ADDR_TXT_SIZE 80	/* IPv4 or IPv6 address*/

#define NFP_CONTROL_CORE (-1)	/* Use the value of 'linux_core_id'*/
#define NFP_DFLT_CLI_CORE (-2)	/* Use the value of
				'nfp_cli_thread_config_t.core_id' */

/**
 * Checksum offloading configuration options bit field
 *
 * Packet IP/UDP/TCP checksum validation and insertion offloading
 * may be enabled or disabled:
 *
 * 0: Disable offloading.
 * 1: Enable offloading. This is the default value.
 *
 * When offloading is disabled, related checksums will be calculated by
 * software, if needed.
 *
 * When offloading is enabled, related checksums will be calculated either
 * by HW (if packet_io supports offloading) or by SW (if packet_io doesn't
 * support offloading), if needed.
 */
typedef struct nfp_chksum_offload_config_t {
	/** Enable IPv4 header checksum validation offload */
	uint16_t ipv4_rx_ena : 1;

	/** Enable UDP checksum validation offload */
	uint16_t udp_rx_ena  : 1;

	/** Enable TCP checksum validation offload */
	uint16_t tcp_rx_ena  : 1;

	/** Enable IPv4 header checksum insertion offload */
	uint16_t ipv4_tx_ena : 1;

	/** Enable UDP checksum insertion offload */
	uint16_t udp_tx_ena  : 1;

	/** Enable TCP checksum insertion offload */
	uint16_t tcp_tx_ena  : 1;
} nfp_chksum_offload_config_t;

/** NFP CLI thread configuration parameters */
typedef struct nfp_cli_thread_config_s {
	/** Start thread on NFP initialization
	 * Default value is 0
	*/
	odp_bool_t start_on_init;

	/** Port where CLI connections are waited.
	 * Default value is NFP_CLI_PORT_DFLT
	*/
	uint16_t port;

	/** Address where CLI connections are waited.
	 * Default value is NFP_CLI_ADDR_DFLT
	*/
	char addr[NFP_CLI_ADDR_TXT_SIZE];

	/** CPU core where CLI thread is pinned.
	 *  Default value is the value of 'linux_core_id'.
	*/
	int core_id;
} nfp_cli_thread_config_t;

/** NFP DNS default (safety belt) NS configuration parameters */
typedef struct nfp_dns_dflt_ns_s {
	/** NS server address */
	char addr[NFP_ADDR_TXT_SIZE];

	/** NS server domain */
	char domain[NFP_DNS_DOMAIN_TXT_SIZE];
} nfp_dns_dflt_ns_t;

typedef struct nfp_dns_hosts_s {
	/** host address */
	char addr[NFP_ADDR_TXT_SIZE];

	/** canonical host name */
	char cname[NFP_DNS_DOMAIN_TXT_SIZE];

	/** alias host names */
	char alias[NFP_DNS_HOSTS_ALIAS_MAX][NFP_DNS_DOMAIN_TXT_SIZE];
} nfp_dns_hosts_t;

/**
 * NFP API initialization data
 *
 * @see nfp_initialize_param()
 */
typedef struct nfp_initialize_param_t {
	/**
	 * ODP instance. The default value is NFP_ODP_INSTANCE_INVALID.
	 * If configured by application, it will be used in subsequent
	 * API calls. Application has the ownership on the instance and
	 * has to cleanup the resources (odp_term_global()).
	 * If not configured by application, NFP will create an ODP
	 * instance with default settings. NFP has ownership on the
	 * instance and will cleanup the resources (odp_term_global())
	 * at nfp_terminate() time.
	 */
	odp_instance_t instance;

	/**
	 * Enable/disable functionalities
	 */
	struct enable_s {
		/** Enable/disable dns functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t dns;

		/** Enable/disable dhcp functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t dhcp;

		/** Enable/disable packet channel functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t channel;

		/** Enable/disable ipsec functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t ipsec;

		/** Enable/disable vlan functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t vlan;

		/** Enable/disable vxlan functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t vxlan;

		/** Enable/disable gre functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t gre;

		/** Enable/disable loopback functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t lo;

		/** Enable/disable CLI functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t cli;

		/** Enable/disable odp classifier functionality.
		 *  The default value is enable (true)
		 */
		odp_bool_t cls;

		/**
		 * Enable/disable netlink functionality.
		 * This option has effect if slow path support is enabled.
		 * The default value is enable (true)
		 */
		odp_bool_t netlink;
	} enable;

	/**
	 * CPU core to which internal NFP control threads are pinned.
	 * The default value is 0.
	 */
	uint16_t linux_core_id;

	/**
	 * CPU cores available for control threads.
	 * Default value is zeroed mask (default CPU mapping is used).
	 * Note: The value is applied only when NFP has ownership on
	 * ODP instance.
	 */
	odp_cpumask_t control_core_mask;

	/**
	 * CPU cores available for worker threads.
	 * Default value is zeroed mask (default CPU mapping is used).
	 * Note: The value is applied only when NFP has ownership on
	 * ODP instance.
	 */
	odp_cpumask_t worker_core_mask;

	/**
	 * Count of interfaces to be initialized. The default value is
	 * 0.
	 */
	uint16_t if_count;

	/**
	 * Names of the interfaces to be initialized. The naming
	 * convention depends on the operating system and the ODP
	 * implementation. Must containg 'if_count' zero terminated
	 * strings.
	 */
	char if_names[NFP_FP_INTERFACE_MAX][NFP_IFNAMSIZ];

	/** Enable/disable the slow path interface management on the
	 * interfaces initialized by NFP.
	 *
	 * Default value is enable (1).
	 */
	odp_bool_t if_sp_mgmt;

	struct ifnet_s {
		/**
		 * MTU to set on interfaces. The actual value is adjusted
		 * based on NIC capabilities and packet pool settings.
		 *
		 * Default value is NFP_MTU_SIZE.
		 */
		uint16_t if_mtu;

		/**
		 * Slow path interface name offset. Slow path interface name for
		 * a network interface is composed from a fix part "sp" and a
		 * variable part formed from interface name offset + interface
		 * index e.g. "sp0".
		 *
		 * Default value is 0.
		 */
		uint16_t if_sp_offset;
	} ifnet;

	/**
	 * Packet input mode of the interfaces initialized by NFP.
	 * Must be ODP_PKTIN_MODE_SCHED if default_event_dispatcher()
	 * is used.
	 *
	 * Default value is ODP_PKTIN_MODE_SCHED.
	 */
	odp_pktin_mode_t pktin_mode;

	/**
	 * Packet output mode of the interfaces initialized by NFP.
	 *
	 * Default value is ODP_PKTOUT_MODE_DIRECT.
	 */
	odp_pktout_mode_t pktout_mode;

	/**
	 * Scheduler synchronization method of the pktin queues of the
	 * interfaces initialized by NFP in the scheduled mode.
	 * Ignored when pktin_mode is not ODP_PKTIN_MODE_SCHED.
	 *
	 * Default value is ODP_SCHED_SYNC_ATOMIC.
	 */
	odp_schedule_sync_t sched_sync;

	/**
	 * ODP event scheduling group for all scheduled event queues
	 * (pktio queues, timer queues and other queues) created in
	 * NFP initialization. The default value is
	 * ODP_SCHED_GROUP_ALL.
	 */
	odp_schedule_group_t sched_group;

	/**
	 * Packet processing hooks. The default value is NULL for
	 * every hook.
	 *
	 * @see nfp_hook.h
	 */
	nfp_pkt_hook pkt_hook[NFP_HOOK_MAX];

	/**
	 * Maximum number of events received at once with the default
	 * event dispatched (default_event_dispatcher()).
	 * Default is NFP_EVT_RX_BURST_SIZE.
	 */
	int evt_rx_burst_size;

	/**
	 * Number of packets sent at once (>= 1).
	 * Default is NFP_PKT_TX_BURST_SIZE
	 */
	uint32_t pkt_tx_burst_size;

	struct pkt_pool_s {
		/** Packet pool size; Default value is SHM_PKT_POOL_NB_PKTS */
		int nb_pkts;

		/**
		 * Packet pool buffer size;
		 * Default value is SHM_PKT_POOL_BUFFER_SIZE
		 */
		unsigned long buffer_size;
	} pkt_pool;

	struct subport_s {
		/**
		 * Maximum number of pseudo-interfaces used to represent
		 * VLAN, VXLAN, GRE and loopback interfaces.
		 * Default value is NFP_SUBPORT_NUM_MAX.
		 */
		int num_max;
	} subport;

	struct route_s {
		/**
		 * IPv4 route mtrie parameters.
		 */
		struct mtrie_s {
			/** Number of IPv4 routes (MTRIE).
			 * Default is NFP_ROUTES. */
			int routes;

			/** Number of 8 bit mtrie nodes.
			 * Default is NFP_MTRIE_TABLE8_NODES.*/
			int table8_nodes;
		} mtrie;

		/** Number of tree nodes used to store IPv4 routes (RADIX).
		 * Default is NFP_ROUTE4_NODES. */
		int route4_nodes;

		/** Number of tree nodes used to store IPv6 routes.
		 * Default is NFP_ROUTE6_NODES. */
		int route6_nodes;

		/**
		 * Maximum number of VRFs. Default is NFP_NUM_VRF.
		 *
		 * VRF IDs used in interfaces and routes must be less than
		 * this value.
		 */
		int num_vrf;

		/**
		 * Speculate the IPv4 default route based on ARP messages,
		 * when a default route is not set.
		 * Default value is false (no speculation).
		 */

		odp_bool_t default_route_arp_spec;

		/**
		 * Add routes on received arp requests/replies.
		 * Default value is true (add).
		 */
		odp_bool_t add_route_on_arp;
	} route;

	/**
	 * Global ARP parameters.
	 */
	struct arp_s {
		/** Maximum number of ARP entries.
		 *  Default is NFP_ARP_ENTRIES.
		 */
		int entries;

		/** ARP hash bits. Default is NFP_ARP_HASH_BITS. */
		int hash_bits;

		/** Entry timeout in seconds.
		 *  Default is NFP_ARP_ENTRY_TIMEOUT.
		 */
		int entry_timeout;

		/**
		 * Timeout (in seconds) for a packet waiting for ARP
		 * to complete. Default is NFP_ARP_SAVED_PKT_TIMEOUT.
		 */
		int saved_pkt_timeout;

		/**
		 * Reply to an ARP request only if the target address of the
		 * request is an address of the receiving interface.
		 * Ignore the request otherwise.
		 *
		 * If not set, reply to an ARP request for any local IP
		 * address regardless of the receiving interface.
		 *
		 * See net.ipv4.conf.all.arp_ignore sysctl in Linux.
		 *
		 * Default value is 0.
		 */
		odp_bool_t check_interface;
	} arp;

	struct timer_s {
		/** Maximum number of timers.
		 *  Default value is NFP_TIMER_NUM_MAX.
		 */
		int num_max;
	} timer;

	/**
	 * Checksum offloading options.
	 */
	nfp_chksum_offload_config_t chksum_offload;

	/*
	 * IPsec parameters
	 */
	struct nfp_ipsec_param ipsec;

	/*
	 * Socket parameters
	 */
	struct socket_s {
		/** Maximum number of sockets */
		uint32_t num_max;

		/** Socket descriptor offset.
		 *  Socket descriptors are returned in interval [sd_offset,
		 *  sd_offset + num_max -1]
		 */
		uint32_t sd_offset;
	} socket;

	/*
	 * Epoll parameters
	 */
	struct epoll_s {
		/** Maximum number of epoll instances.
		 * Default value is NFP_EPOLL_NUM_MAX.
		 */
		uint32_t num_max;

		/** Maximum number of socket descriptors that can be
		 * monitored by an epoll instance
		 * Default value is NFP_EPOLL_SET_SIZE.
		 */
		uint32_t set_size;
	} epoll;

	/*
	* IPv4 multicast
	*/
	struct in_mcast_s {
		/**
		* Maximum number of multicast aware PCBs.
		* Default value is NFP_NUM_PCB_MCAST_MAX.
		*/
		uint32_t pcb_mcast_max;

		/**
		 * Maximum number of multicast group memberships (all PCBs).
		 * Default value is NFP_MCAST_MEMBERSHIP_MAX.
		 */
		uint32_t grp_membership_max;

		/**
		 * Maximum number of multicast sources for all PCBs.
		 * Default value is NFP_MCAST_SOURCE_MAX.
		 */
		uint32_t src_max;

		/**
		 * Maximum number of multicast addresses assigned
		 * to all interfaces.
		 * Default value is NFP_MCAST_IF_MADDR_MAX.
		 */
		uint32_t if_maddr_max;
	} in_mcast;

	/*
	 * TCP parameters
	 */
	struct tcp_s {
		/**
		 * Maximum number of TCP PCBs.
		 * Default value is NFP_NUM_PCB_TCP_MAX
		 */
		int pcb_tcp_max;

		/**
		 * Size of pcb hash.
		 * Must be a power of 2.
		 */
		int pcb_hashtbl_size;

		/**
		 * Size of pcbport hash.
		 * Must be a power of 2.
		 */
		int pcbport_hashtbl_size;

		/**
		 * Size of syncache hash.
		 * Must be a power of 2.
		 */
		int syncache_hashtbl_size;

		/**
		 * Maximum number of SACK holes.
		 * Default value is 4 * pcb_tcp_max
		 */
		int sackhole_max;

		/**
		 * Maximum number of PHDs (local ports).
		 * Default value is pcb_tcp_max.
		 */
		int phd_max;
	} tcp;

	/*
	 * UDP parameters
	 */
	struct udp_s {
		/**
		 * Maximum number of UDP PCBs.
		 * Default value is NFP_NUM_PCB_UDP_MAX
		 */
		int pcb_udp_max;

		/**
		 * Size of pcb hash.
		 * Must be a power of 2.
		 */
		int pcb_hashtbl_size;

		/**
		 * Size of pcbport hash
		 * Must be a power of 2.
		 */
		int pcbport_hashtbl_size;

		/**
		 * Maximum number of PHDs (local ports).
		 * Default value is pcb_udp_max.
		 */
		int phd_max;
	} udp;

	/*
	 * ICMP parameters
	 */
	struct icmp_s {
		/**
		 * Maximum number of ICMP PCBs.
		 * Default value is NFP_NUM_PCB_ICMP_MAX
		 */
		int pcb_icmp_max;
	} icmp;

	/**
	 * Create default loopback interface lo0, 127.0.0.1/8.
	 * Interface can also be created with CLI or
	 * nfp_ifport_loopback_ipv4_up() API.
	 */
	odp_bool_t if_loopback;

	/**
	 * Log level
	 */
	enum nfp_log_level_s loglevel;

	/**
	 * Debug parameters
	 */
	struct debug_s {
		/**
		 * Bitmask options for printing traffic on file (and console) in
		 * text format and capturing traffic on file in pcap format.
		 *	bit 0: print packets from NIC to FP.
		 *		Use NFP_DEBUG_PRINT_RECV_NIC to set this flag.
		 *	bit 1: print packets from FP to NIC.
		 *		Use NFP_DEBUG_PRINT_SEND_NIC to set this flag.
		 *	bit 2: print packets from FP to SP.
		 *		Use NFP_DEBUG_PRINT_RECV_KNI to set this flag.
		 *	bit 3: print packets from SP to NIC.
		 *		Use NFP_DEBUG_PRINT_SEND_KNI to set this flag.
		 *	bit 4: print packets to console.
		 *		Use NFP_DEBUG_PRINT_CONSOLE to set this flag.
		 *	bit 6: capture packets to pcap file.
		 *		Use NFP_DEBUG_CAPTURE to set this flag.
		 */
		int flags;

		/**
		 * Name of the file where the packets are printed (text format)
		 * Default value is given by DEFAULT_DEBUG_TXT_FILE_NAME macro.
		 */
		char print_filename[NFP_FILE_NAME_SIZE_MAX];

		/**
		 * Bitmask of the ports for which the packets are captures
		 */
		int capture_ports;

		/**
		 * Name of the file where the packets are captured (pcap format)
		 * Default value is given by DEFAULT_DEBUG_PCAP_FILE_NAME macro.
		 */
		char capture_filename[NFP_FILE_NAME_SIZE_MAX];
	} debug;

	/**
	 * CLI parameters
	*/
	struct cli_s {
		/** Parameters coresponding to the CLI thread using OS sockets
		 * and bound address */
		nfp_cli_thread_config_t os_thread;

		/** Parameters coresponding to the CLI thread using NFP sockets
		 * and bound address */
		nfp_cli_thread_config_t nfp_thread;

		/**
		 * Enable execution of shutdown command.
		 * If set to true, the command will stop the execution of NFP
		 * internal threads and also user created NFP control and
		 * worker threads or processes (if were constructed to inspect
		 * the processing state of NFP (see nfp_get_processing_state()).
		 * If set to false, the shutdown cli command will report an
		 * error and operation will not take place.
		 * Default value is true.
		*/
		odp_bool_t enable_shutdown_cmd;
	} cli;

	struct channel_s {
		/**
		 * Maximum number of channels.
		 * Default value is NFP_CHANNEL_NUM_MAX.
		*/
		uint32_t num_max;
	} channel;

	struct dhcp_s {
		/**
		 * Maximum number of DHCP instances.
		 * Default value is NFP_DHCP_NUM_MAX.
		*/
		uint32_t num_max;
	} dhcp;

	struct dns_s {
		/**
		 * List of default (safety belt) NS servers
		 * Default value is empty list.
		*/
		nfp_dns_dflt_ns_t dflt_ns[NFP_DNS_DFLT_NS_MAX];

		/**
		 * Domain static lookup table
		 */
		nfp_dns_hosts_t hosts[NFP_DNS_HOSTS_MAX];

		/**
		 * Maximum time (seconds) to wait for a query response.
		 * Default value is NFP_DNS_QUERY_TIMEOUT.
		*/
		uint32_t query_timeout;

		/**
		 * Maximum number of query retries.
		 * Default value is NFP_DNS_QUERY_RETRY_MAX.
		 */
		uint32_t query_retry_max;

		/**
		 * Maximum number of resolver concurent instances.
		 * Default value is NFP_DNS_RESOVER_MAX.
		 */
		uint32_t resolver_max;

		/**
		 * Maximum number of cache elements.
		 * Default value is NFP_DNS_CACHE_MAX.
		 */
		uint32_t cache_max;
	} dns;
} nfp_initialize_param_t;

/**
 * NFP parameters
 *
 * @see nfp_get_parameters()
 */
typedef struct nfp_param_t {
	/**
	 * NFP API initialization data
	 */
	nfp_initialize_param_t global_param;
} nfp_param_t;

/**
 * Initialize nfp_initialize_param_t to its default values.
 *
 * This function should be called to initialize the supplied parameter
 * structure to default values before setting application specific values
 * and before passing the parameter structure to nfp_initialize().
 *
 * Using this function makes the application to some extent forward
 * compatible with future versions of NFP that may add new fields in
 * the parameter structure.
 *
 * If libconfig is enabled, a configuration file may be used. The
 * configuration file location may be set using the environment
 * variable NFP_CONF_FILE. If the environment variable is not set, the
 * file is read from $(sysconfdir)/nfp.conf, normally
 * /usr/local/etc/nfp.conf.
 *
 * See conf/README file for the configuration file detailed description.
 *
 * @param params structure to initialize
 *
 * @see nfp_initialize()
 */
void nfp_initialize_param(nfp_initialize_param_t *params);

/**
 * Initialize nfp_initialize_param_t according to a configuration file.
 *
 * This function is similar to nfp_initialize_param(), but allows the
 * caller to specify the location of the configuration file. Calling
 * this function with filename = NULL has the same effect as calling
 * nfp_initialize_param(). Passing a zero-length string as filename
 * means that no configuration file will be used, not even the default
 * or the file specified by the environment variable.
 *
 * @see nfp_initialize_param()
 *
 * @param params structure to initialize
 * @param filename name of the configuration file
 */
void nfp_initialize_param_from_file(nfp_initialize_param_t *params,
				    const char *filename);

/**
 * NFP initialization
 *
 * This function must be called only once for an application before
 * calling any other NFP API functions.
 *
 * If an ODP instance is provided as argument, it has to be called
 * from an ODP control thread.
 * If an ODP instance is not provided as argument, it will create an
 * ODP instance and will initialize current thread as control thread.
 *
 * @param params Structure with parameters for NFP initialization.
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
int nfp_initialize(nfp_initialize_param_t *params);

/**
 * NFP termination
 *
 * This function must be called only once in an NFP control
 * thread before exiting application.
 *
 * Should be called from a thread within the same schedule group specified in
 * the parameters of nfp_initialize().
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
int nfp_terminate(void);

/**
 * Thread or process resources initialization
 *
 * This API is called by functions like nfp_initialize(),
 * nfp_thread_create() and nfp_process_fork_n() to initialize thread or
 * process local resources.
 *
 * Application should not call this function unless it uses ODP API
 * directly to create threads or processes. In that case it must
 * call it before calling any other NFP API on that thread or process.
 *
 * @param description Thread or process short description. Takes a null
 * terminated string value or NULL if no description is given.
 *
 * @retval 0 on success
 * @retval -1 on failure
 *
 * @see nfp_initialize() which must have been called prior to this.
 */
int nfp_init_local_resources(const char *description);

/**
 * Thread or process resources termination
 *
 * This API is called by NFP to cleanup local resources before
 * exiting a thread or process.
 *
 * Application should not call this function unless it used ODP API
 * directly to create threads or processes. In that case it should
 * call it after last NFP API of the thread was called.
 *
 * @retval 0 on success
 * @retval -1 on failure
 *
 * @see nfp_terminate() which may be called after this.
 */
int nfp_term_local_resources(void);

/**
 * Stop packet processing
 *
 * Stop processing threads
 *
 *
 * @retval nfp_get_processing_state() which may be called get
 *         the processing state
 *
 *
 * @see
 */
void nfp_stop_processing(void);

/**
 * Get address of processing state variable
 *
 * All processing loops should stop when
 * processing state turns 0
 *
 * @retval non NULL on success
 * @retval NULL on failure
 *
 * @see nfp_stop_processing() which may be called to stop the
 *      processing.
 */

odp_bool_t *nfp_get_processing_state(void);

/**
 * Get NFP parameters
 *
 * @param params Structure to be filled with NFP global parameters
 *
 * @retval 0 on success
 * @retval -1 on failure
 */

int nfp_get_parameters(nfp_param_t *params);

/**
 * Get ODP instance
 *
 * @retval ODP instance on success
 * @retval NFP_ODP_INSTANCE_INVALID on error
 */

odp_instance_t nfp_get_odp_instance(void);

#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_INIT_H__ */
