/* Copyright (c) 2015, ENEA Software AB
 * Copyright (c) 2015, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_CONFIG_H__
#define __NFP_CONFIG_H__

/**
 * @file
 *
 * @brief Configuration defaults for NFP
 *
 */

/* Enable features */

/**Enable static socket configuration mode.
 * It is meant to be used with application where the socket
 * configuration does not change during intensive packet
 * processing phase of application.
 * The purpose is to increase performance by removing locks.
 * Application run time is divided in two phases:
 *  - Initialization phase: all socket creation and configuration
 *    is done during this phase. It starts after completion of
 *    NFP global/local initialization and may expand until after
 * creation of dispatched threads.
 *  - Intensive packet processing phase: no new sockets / binds
 *    are permitted.
 * Restrictions:
 *  During "Initialization phase", all socket operations must be
 *  serialized.
 *  During "Intensive packet processing phase", calls to
 *  nfp_socket(), nfp_bind(), nfp_accept(), nfp_connect(),
 *  nfp_listen(), nfp_shutdown() and nfp_close() are forbidden.
 *  Also, socket must be bound (preferably not to NFP_INADDR_ANY).
 * Implementation details:
 *   Implementation is based on disabling locks on UDP and TCP
 *   hash of PCBs. When NFP_STATIC_SOCKET_CONFIG is defined then
 *   protocols PCB hash will not be protected against concurrent
 *   adding or removing of items.
 */
/* #define NFP_STATIC_SOCKET_CONFIG */

/**NFP configured to send ICMP redirect*/
/* #define NFP_SEND_ICMP_REDIRECT */

/* Configure values */
/** Packet pool size. */
# define SHM_PKT_POOL_NB_PKTS		10240

/** Packet pool buffer size. */
#define SHM_PKT_POOL_BUFFER_SIZE	1856
/** Packet pool name. */
#define SHM_PKT_POOL_NAME "packet_pool"

/** Maximum size of transmitted IP datagram fragments. */
#define NFP_MTU_SIZE 1500

/**Socket handle values returned are in the interval:
 * [NFP_SOCK_NUM_OFFSET, NFP_SOCK_NUM_OFFSET + NFP_NUM_SOCKETS_MAX] */

/**Maximum number of sockets. */
#define NFP_NUM_SOCKETS_MAX 1024

/**First socket number value. */
#define NFP_SOCK_NUM_OFFSET 0

/**Maximum number of multicast PCBs. */
#define NFP_NUM_PCB_MCAST_MAX 8

/**Maximum number of multicast group memberships per PCB. */
#define NFP_MCAST_MEMBERSHIP_PCB_MAX 8

/**Maximum number of multicast group memberships. */
#define NFP_MCAST_MEMBERSHIP_MAX \
	(NFP_NUM_PCB_MCAST_MAX * NFP_MCAST_MEMBERSHIP_PCB_MAX)

/**Maximum number of multicast IP sources. */
#define NFP_MCAST_SOURCE_MAX 256

/**Maximum number of multicast addresses assigned to interfaces */
#define NFP_MCAST_IF_MADDR_MAX NFP_MCAST_SOURCE_MAX

/**Maximum number of TCP PCBs. */
#define NFP_NUM_PCB_TCP_MAX 1024

/**Maximum number of TCP SACK holes. */
#define NFP_SACK_HOLE_MAX (4 * NFP_NUM_PCB_TCP_MAX)

/**Maximum number of UDP PCBs. */
#define NFP_NUM_PCB_UDP_MAX 1024

/**Maximum number of ICMP PCBs. */
#define NFP_NUM_PCB_ICMP_MAX 8

#define NFP_TCP_MAX_CONNECTION_RATE

/**Maximum number of epoll instances. */
#define NFP_EPOLL_NUM_MAX 8

/** Epoll set size */
#define NFP_EPOLL_SET_SIZE 512

/**Select set size
 * select() can monitor only socket descriptor numbers that are
 * less then NFP_SELECT_SET_SIZE + 'socket number offset' (that is
 * NFP_SOCK_NUM_OFFSET or configured runtime value).
*/
#define NFP_SELECT_SET_SIZE 65536

/**Maximum number of fastpath interfaces used.
 * For each fastpath interface a PKTIO in opened by NFP.*/
#define NFP_FP_INTERFACE_MAX 8

/* Maximum number of pseudo-interfaces used to represent
 * VLAN, VXLAN, GRE and loopback interfaces.*/
#define NFP_SUBPORT_NUM_MAX 256

/* Maximum number of IPv4 addresses per interfaces */
#define NFP_NUM_IFNET_IP_ADDRS 8

/* Maximum number of IPv6 addresses per interfaces */
#define NFP_NUM_IFNET_IP6_ADDRS 4

/**Maximum number of output queues that can be configured for an
 * NFP interface*/
#define NFP_PKTOUT_QUEUE_MAX 64

/** Maximum number of packets in a packet vector */
#define NFP_PKT_VECTOR_SIZE 32

/**Maximum number of events received at once in scheduling mode
 * in default_event_dispatcher() or default_event_dispatcher_multi().*/
#define NFP_EVT_RX_BURST_SIZE NFP_PKT_VECTOR_SIZE

/**Number of packets sent at once (>= 1)   */
#define NFP_PKT_TX_BURST_SIZE 1

/**Controls memory size for IPv4 MTRIE 16/8/8 data structure.
 * It defines the number of small tables (8) used to store routes.*/
#define NFP_MTRIE_TABLE8_NODES 128
/** Defines the maximum number of routes that are stored in the MTRIE.*/
#define NFP_ROUTES 65536

/** Number of VRFs. */
#define NFP_NUM_VRF 1

/**Controls memory size for IPv4 radix tree data structure.
 * It defines the number of radix tree nodes used to store routes.*/
#define NFP_ROUTE4_NODES 65536

/**Controls memory size for IPv6 tree data structure.
 * It defines the number of tree nodes used to store routes.*/
#define NFP_ROUTE6_NODES 65536

/**ARP hash bits. */
#define NFP_ARP_HASH_BITS 11
/**Total number of ARP entries that can be stored. */
#define NFP_ARP_ENTRIES 128
/**Default ARP entry timeout (in seconds). */
#define NFP_ARP_ENTRY_TIMEOUT 1200
/**Time interval (s) while a packet is saved and waiting for an ARP reply. */
#define NFP_ARP_SAVED_PKT_TIMEOUT 10

/**Total number of IP6 packets that can be stored for Neighbor Discovery. */
#define NFP_NDP_HOLD_PKTS 2048
/**Maximum number of IP6 packets that can be stored per neighbor. */
#define NFP_NDP_HOLD_PKTS_NEIGHBOR 8

/**Maximum number of concurent NDP timeout operations
 * NDP timeout operations include neighbor solicitation retransmissions and
 * prefixes and routers expirations.*/
#define NFP_NDP_TIMEOUT_OPS_ENTRIES 256

/**Time interval (s) between NDP timeout operations status checks */
#define NFP_NDP_TIMEOUT_OPS_TIMER 1

/**Maximum number of timers*/
#define NFP_TIMER_NUM_MAX 10000

/**Enable IPv4 UDP checksum validation mechanism on input
 * packets. If enabled, validation is performed on input
 * packets. */
#define NFP_IPv4_UDP_CSUM_VALIDATE

/**Enable IPv4 UDP checksum computation mechanism for outgoing
 * packets. If enabled, computation is performed based on
 * sysctl() option net.inet.udp.checksum (default: compute
 * checksum). */
#define NFP_IPv4_UDP_CSUM_COMPUTE

/**Enable IPv4 TCP checksum validation mechanism on input
 * packets. If enabled, validation is performed on input
 * packets. */
#define NFP_IPv4_TCP_CSUM_VALIDATE

/**Enable (1) or disable (0) offloading of IPv4/UDP/TCP checksum
 * validation and insertion. If enabled, checksum calculation will
 * be performed by HW, if possible. See nfp_chksum_offload_config_t.*/
#define NFP_CHKSUM_OFFLOAD_IPV4_RX 1
#define NFP_CHKSUM_OFFLOAD_UDP_RX  1
#define NFP_CHKSUM_OFFLOAD_TCP_RX  1
#define NFP_CHKSUM_OFFLOAD_IPV4_TX 1
#define NFP_CHKSUM_OFFLOAD_UDP_TX  1
#define NFP_CHKSUM_OFFLOAD_TCP_TX  1

/** Maximum size of a file name */
#define NFP_FILE_NAME_SIZE_MAX 256

/** Maximum size of an interface name */
#define NFP_IFNAMSIZ  32

/** Maximum size of the thread or process description */
#define NFP_THREAD_DESCR_SIZE_MAX  8

/** NFP default CLI port */
#define NFP_CLI_PORT_DFLT 2345

/** NFP default CLI address */
#define NFP_CLI_ADDR_DFLT "0.0.0.0"

/** Maximum number of channels. */
#define NFP_CHANNEL_NUM_MAX 32

/** Maximum number of packets sent at once to a channel*/
#define NFP_CHANNEL_SEND_MAX NFP_PKT_VECTOR_SIZE

/** Maximum number of packets received once from a channel.*/
#define NFP_CHANNEL_RECV_MAX NFP_PKT_VECTOR_SIZE

/** Maximum number of DHCP instances. */
#define NFP_DHCP_NUM_MAX 16

/** Maximum number of DHCP6 instances. */
#define NFP_DHCP6_NUM_MAX 16

/** Maximum number of default (safety belt) NS servers. */
#define NFP_DNS_DFLT_NS_MAX 3

/** Maximum number of host lines in static lookup table. */
#define NFP_DNS_HOSTS_MAX 4

/** Maximum number of aliases per host line. */
#define NFP_DNS_HOSTS_ALIAS_MAX 2

/** Maximum time (seconds) to wait for a query response. */
#define NFP_DNS_QUERY_TIMEOUT 2

/** Maximum number of query retries. */
#define NFP_DNS_QUERY_RETRY_MAX 3

/** Maximum number of resolver concurent instances */
#define NFP_DNS_RESOVER_MAX 5

/** Maximum number of name servers in the query list of
 *  an resolver instance */
#define NFP_DNS_RESOVER_SLIST_MAX 4

/** Maximum number of cache elements */
#define NFP_DNS_CACHE_MAX (NFP_DNS_HOSTS_MAX + 20)

/** Maximum number of addresses stored in a cache element */
#define NFP_DNS_CACHE_ADDR_MAX 8

/** Maximum number of name servers in a cache element */
#define NFP_DNS_CACHE_NS_MAX 4

#endif
