#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "nfp.h"

#include "mcast.h"

static int mcasttest(void *arg)
{
	int fd;
	uint32_t addr_local = 0;
	uint32_t addr_mcast = 0;
	struct nfp_ip_mreq mreq;
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	odp_bool_t *is_running = NULL;
	char buf[1024];
	nfp_size_t buf_len = sizeof(buf);
	nfp_ssize_t len = 0;
	struct nfp_sockaddr_in addr_bind;
	struct nfp_sockaddr_in addr;
	nfp_socklen_t addr_len = 0;

	(void)arg;

	NFP_INFO("Multicast thread started\n");

	is_running = nfp_get_processing_state();
	if (is_running == NULL) {
		NFP_ERR("nfp_get_processing_state failed");
		return -1;
	}

	/* Address local */
	ifnet = nfp_ifport_ifnet_get(0, NFP_IFPORT_NET_SUBPORT_ITF);
	if (ifnet == NFP_IFNET_INVALID) {
		NFP_ERR("Interface not found.");
		return -1;
	}

	while (addr_local == 0) {
		if (nfp_ifnet_ipv4_addr_get(ifnet, NFP_IFNET_IP_TYPE_IP_ADDR,
					    &addr_local)) {
			NFP_ERR("Error: Failed to get IP address.");
			return -1;
		}
		sleep(1);
	}

	/* Address mcast */
	if (!nfp_parse_ip_addr(APP_ADDR_MCAST, &addr_mcast)) {
		NFP_ERR("Failed to get MCAST address.");
		return -1;
	}

	fd = nfp_socket(NFP_AF_INET, NFP_SOCK_DGRAM, NFP_IPPROTO_UDP);
	if (fd < 0) {
		NFP_ERR("Cannot open socket!\n");
		return -1;
	}

	/* Bind on local address */
	memset(&addr_bind, 0, sizeof(addr_bind));
	addr_bind.sin_family = NFP_AF_INET;
	addr_bind.sin_port = odp_cpu_to_be_16(APP_PORT);
	addr_bind.sin_addr.s_addr = 0;
	addr_bind.sin_len = sizeof(addr_bind);

	if (nfp_bind(fd, (struct nfp_sockaddr *)&addr_bind,
		     sizeof(struct nfp_sockaddr)) < 0) {
		NFP_ERR("Cannot bind socket (%s)!\n", nfp_strerror(nfp_errno));
		nfp_close(fd);
		return -1;
	}

	/* Mcast group membership */
	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = addr_mcast;
	mreq.imr_interface.s_addr = addr_local;
	if (nfp_setsockopt(fd, NFP_IPPROTO_IP, NFP_IP_ADD_MEMBERSHIP,
			   &mreq, sizeof(mreq)) == -1) {
		NFP_ERR("nfp_setsockopt() failed: %d.", nfp_errno);
		nfp_close(fd);
		return -1;
	}

	while (*is_running) {
		addr_len = sizeof(addr);
		len = nfp_recvfrom(fd, buf, buf_len - 1, 0,
				   (struct nfp_sockaddr *)&addr, &addr_len);
		if (len == -1) {
			NFP_ERR("Failed to rcv data(errno = %d)\n", nfp_errno);
			continue;
		}

		buf[len] = 0;
		NFP_INFO("Data (%s, len = %d) was received.\n", buf, len);

		if (addr_len != sizeof(addr)) {
			NFP_ERR("Failed to rcv source address: %d (errno = %d)",
				addr_len, nfp_errno);
			continue;
		}

		NFP_INFO("Data was received from address %s, port = %d.\n",
			 nfp_print_ip_addr(addr.sin_addr.s_addr),
			 odp_be_to_cpu_16(addr.sin_port));

		sprintf(buf, "%d bytes", len);

		if (nfp_sendto(fd, buf, strlen(buf), 0,
			       (struct nfp_sockaddr *)&addr,
			       sizeof(addr)) == -1) {
			NFP_ERR("Failed to send data (errno = %d)\n",
				nfp_errno);
		}
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.imr_multiaddr.s_addr = addr_mcast;
	mreq.imr_interface.s_addr = addr_local;
	if (nfp_setsockopt(fd, NFP_IPPROTO_IP, NFP_IP_DROP_MEMBERSHIP,
			   &mreq, sizeof(mreq)) == -1) {
		NFP_ERR("nfp_setsockopt() failed: %d.",
			nfp_errno);
	}

	nfp_close(fd);
	NFP_INFO("Multicast thread ended");
	return 0;
}

int nfp_multicast_thread(nfp_thread_t *thread_mcast, int core_id)
{
	odp_cpumask_t cpumask;
	nfp_thread_param_t thread_param = {0};

	odp_cpumask_zero(&cpumask);
	odp_cpumask_set(&cpumask, core_id);

	nfp_thread_param_init(&thread_param);
	thread_param.start = mcasttest;
	thread_param.arg = NULL;
	thread_param.thr_type = ODP_THREAD_CONTROL;

	return nfp_thread_create(thread_mcast, 1,
			       &cpumask, &thread_param);
}
