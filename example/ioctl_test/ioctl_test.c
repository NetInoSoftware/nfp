/*-
 * Copyright (c) 2014 ENEA Software AB
 * Copyright (c) 2014 Nokia
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "nfp.h"

#include "ioctl_test.h"

#define logfilename "/tmp/iocrl-test.log"
static FILE *logfile;

#define IFNAME "fp0"
#define GRENAME "gre1"

#define IP4(a,b,c,d) (a|(b<<8)|(c<<16)|(d<<24))

static uint32_t
get_ip_address(int fd, const char *name)
{
	struct nfp_ifreq ifr;

	strcpy(ifr.ifr_name, name);
	if (nfp_ioctl(fd, NFP_SIOCGIFADDR, &ifr) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
		return 0;
	} else {
		struct nfp_sockaddr_in *ipaddr;
		ipaddr = (struct nfp_sockaddr_in *)&ifr.ifr_addr;
		return ipaddr->sin_addr.s_addr;
	}
}

static uint32_t
get_netmask(int fd, const char *name)
{
	struct nfp_ifreq ifr;

	strcpy(ifr.ifr_name, name);
	if (nfp_ioctl(fd, NFP_SIOCGIFNETMASK, &ifr) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
		return 0;
	} else {
		struct nfp_sockaddr_in *ipaddr;
		ipaddr = (struct nfp_sockaddr_in *)&ifr.ifr_addr;
		return ipaddr->sin_addr.s_addr;
	}
}

static uint32_t
get_broadcast_address(int fd, const char *name)
{
	struct nfp_ifreq ifr;

	strcpy(ifr.ifr_name, name);
	if (nfp_ioctl(fd, NFP_SIOCGIFBRDADDR, &ifr) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
		return 0;
	} else {
		struct nfp_sockaddr_in *ipaddr;
		ipaddr = (struct nfp_sockaddr_in *)&ifr.ifr_addr;
		return ipaddr->sin_addr.s_addr;
	}
}

static void
set_ip_address_and_mask(int fd, const char *name, uint32_t addr, uint32_t mask)
{
	struct nfp_in_aliasreq ifra;

	strcpy(ifra.ifra_name, name);
	ifra.ifra_addr.sin_family = NFP_AF_INET;
	ifra.ifra_addr.sin_addr.s_addr = addr;
	ifra.ifra_mask.sin_addr.s_addr = mask;
	if (nfp_ioctl(fd, NFP_SIOCSIFADDR, &ifra) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static void
delete_if_address(int fd, const char *name)
{
	struct nfp_in_aliasreq ifra;

	strcpy(ifra.ifra_name, name);
	ifra.ifra_addr.sin_family = NFP_AF_INET;
	if (nfp_ioctl(fd, NFP_SIOCDIFADDR, &ifra) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static void
receive_non_blocking(void)
{
	int s, ret, nb = 1;
	struct nfp_sockaddr_in addr;
	struct nfp_sockaddr_in remote;
	nfp_socklen_t remote_len;
	char buf[64];
	nfp_size_t len = sizeof(buf);

	s = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);

	addr.sin_family = NFP_AF_INET;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = odp_cpu_to_be_16(2048);
	addr.sin_len = sizeof(addr);

	if ((ret = nfp_bind(s, (struct nfp_sockaddr *)&addr, sizeof(addr)))) {
		NFP_ERR("nfp_bind failed ret=%d %s", ret, nfp_strerror(nfp_errno));
	}

	/*
	 * Set non-blocking mode.
	 */
	if (nfp_ioctl(s, NFP_FIONBIO, &nb) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}

	/*
	 * No data expected. Immediate return.
	 */
	remote_len = sizeof(remote);
	ret = nfp_recvfrom(s, buf, len, 0,
			     (struct nfp_sockaddr *)&remote, &remote_len);
	nfp_close(s);
}

static int
get_sockbuf_data(int fd, uint32_t cmd)
{
	int val;
	if (nfp_ioctl(fd, cmd, &val) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
	return val;
}

static void
get_if_conf(int fd, struct nfp_ifconf *conf)
{
	if (nfp_ioctl(fd, NFP_SIOCGIFCONF, conf) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static void
set_gre_tunnel(int fd, const char *name, uint32_t addr, uint32_t p2p,
	       uint32_t local, uint32_t remote, int vrf)
{
	struct nfp_in_tunreq treq;

	strcpy(treq.iftun_name, name);
	treq.iftun_addr.sin_addr.s_addr = addr;
	treq.iftun_p2p_addr.sin_addr.s_addr = p2p;
	treq.iftun_local_addr.sin_addr.s_addr = local;
	treq.iftun_remote_addr.sin_addr.s_addr = remote;
	treq.iftun_vrf = vrf;
	treq.iftun_sp_itf_mgmt = 1;

	if (nfp_ioctl(fd, NFP_SIOCSIFTUN, &treq) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static void
set_vrf(int fd, const char *name, int vrf)
{
	struct nfp_ifreq ifr;

	strcpy(ifr.ifr_name, name);
	ifr.ifr_fib = vrf;

	if (nfp_ioctl(fd, NFP_SIOCSIFFIB, &ifr) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static void
set_route(int fd, const char *dev, int vrf,
	  uint32_t dst, uint32_t mask, uint32_t gw)
{
	struct nfp_rtentry rt;

	rt.rt_vrf = vrf;
	rt.rt_dev = (char *)(uintptr_t)dev;
	((struct nfp_sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = dst;
	((struct nfp_sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = mask;
	((struct nfp_sockaddr_in *)&rt.rt_gateway)->sin_addr.s_addr = gw;

	if (nfp_ioctl(fd, NFP_SIOCADDRT, &rt) < 0) {
		NFP_ERR("nfp_ioctl failed, err='%s'",
			nfp_strerror(nfp_errno));
	}
}

static int
ioctl_test(void *arg)
{
	int fd;
	uint32_t addr, origaddr, origmask;

	(void)arg;

	logfile = fopen(logfilename, "w");
	NFP_INFO("ioctl_test thread started");

	sleep(2);

	if ((fd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP)) < 0) {
		NFP_ERR("nfp_socket failed, err='%s'",
			nfp_strerror(nfp_errno));
		return -1;
	}

	NFP_INFO("=====================================");
	NFP_INFO("Get IP address of %s", IFNAME);
	origaddr = get_ip_address(fd, IFNAME);
	NFP_INFO("  %s", nfp_print_ip_addr(origaddr));

	NFP_INFO("=====================================");
	NFP_INFO("Get netmask of %s", IFNAME);
	origmask = get_netmask(fd, IFNAME);
	NFP_INFO("  %s", nfp_print_ip_addr(origmask));

	NFP_INFO("=====================================");
	NFP_INFO("Get broadcast address of %s", IFNAME);
	NFP_INFO("  %s",
		 nfp_print_ip_addr(get_broadcast_address(fd, IFNAME)));

	NFP_INFO("=====================================");
	NFP_INFO("Delete IP address of %s", IFNAME);
	delete_if_address(fd, IFNAME);

	NFP_INFO("=====================================");
	addr = IP4(192,168,156,111);
	NFP_INFO("Set IP address of %s to %s/%d",
		 IFNAME, nfp_print_ip_addr(addr), 25);
	set_ip_address_and_mask(fd, IFNAME, addr, odp_cpu_to_be_32(0xffffff80));

	NFP_INFO("Set back original address and mask");
	set_ip_address_and_mask(fd, IFNAME, origaddr, origmask);

	NFP_INFO("=====================================");
	NFP_INFO("Receiving from socket");
	receive_non_blocking();
	NFP_INFO("Immediate return");

	NFP_INFO("=====================================");
	NFP_INFO("Get sockbuf bytes to read");
	NFP_INFO("  %d", get_sockbuf_data(fd, NFP_FIONREAD));

	NFP_INFO("=====================================");
	NFP_INFO("Get sockbuf bytes yet to write");

	NFP_INFO("  %d", get_sockbuf_data(fd, NFP_FIONWRITE));

	NFP_INFO("=====================================");
	NFP_INFO("Get sockbuf send space");

	NFP_INFO("  %d", get_sockbuf_data(fd, NFP_FIONSPACE));

	NFP_INFO("=====================================");
	NFP_INFO("Set GRE tunnel");
	set_gre_tunnel(fd, GRENAME, IP4(10,3,4,1), IP4(10,3,4,2),
		       origaddr, IP4(192,168,56,104), 0);

	NFP_INFO("=====================================");
	NFP_INFO("Change GRE tunnel's VRF");
	set_vrf(fd, GRENAME, 7);

	NFP_INFO("=====================================");
	NFP_INFO("Get all interfaces");

	struct nfp_ifconf conf;
	char data[1024];
	struct nfp_ifreq *ifr;
	int i = 1;

	conf.ifc_len = sizeof(data);
	conf.ifc_buf = (char *)data;

	get_if_conf(fd, &conf);

	ifr = (struct nfp_ifreq *)data;
	while ((char *)ifr < data + conf.ifc_len) {
		switch (ifr->ifr_addr.sa_family) {
		case NFP_AF_INET:
			NFP_INFO("  %d. %s : %s", i, ifr->ifr_name,
				 nfp_print_ip_addr(((struct nfp_sockaddr_in *)
						      &ifr->ifr_addr)->sin_addr.s_addr));
			break;
		}
		ifr++;
		i++;
	}

	NFP_INFO("=====================================");
	NFP_INFO("Set routes");

	set_route(fd, GRENAME, 0, IP4(10,1,1,0), IP4(255,255,255,0), IP4(10,3,4,2));
	/*
	 * If output device is not set it will be found using the route to gateway.
	 */
	set_route(fd, NULL, 0, IP4(10,7,0,0), IP4(255,255,0,0), IP4(192,168,56,254));

	NFP_INFO("=====================================");
	nfp_close(fd);
	NFP_INFO("Ioctl test exit");
	NFP_INFO("=====================================");

	fclose(logfile);
	if (system("cat " logfilename) < 0)
		NFP_ERR("system failed");

	nfp_stop_processing();

	return 0;
}

int  nfp_start_ioctl_thread(nfp_thread_t *thread_ioctl, int core_id)
{
	odp_cpumask_t cpumask;
	nfp_thread_param_t thread_param = {0};

	odp_cpumask_zero(&cpumask);
	odp_cpumask_set(&cpumask, core_id);

	nfp_thread_param_init(&thread_param);
	thread_param.start = ioctl_test;
	thread_param.arg = NULL;
	thread_param.thr_type = ODP_THREAD_CONTROL;

	return nfp_thread_create(thread_ioctl, 1,
			       &cpumask,
			       &thread_param);
}
