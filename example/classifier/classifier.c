/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#include "classifier.h"
#include "nfp_log.h"

static odp_cos_t build_cos_w_queue(const char *name,
				   odp_schedule_group_t group_cls);
static odp_pmr_t build_udp_prm(odp_cos_t cos_src, odp_cos_t cos_dst);

void init_classifier(classif_t *classif)
{
	int i;

	classif->cos_udp = ODP_COS_INVALID;
	classif->pmr_udp = ODP_PMR_INVALID;
	for (i = 0; i < NFP_FP_INTERFACE_MAX; i++)
		classif->cos_def[i] = ODP_COS_INVALID;
}

int build_classifier(appl_arg_ifs_t *itf_param,
		     odp_schedule_group_t group_cls,
		     odp_schedule_group_t group_workers,
		     classif_t *classif)
{
	odp_pktio_t pktio;
	odp_pmr_t pmr_udp;
	char name[80];
	int i, port;
	nfp_ifnet_t ifnet = NFP_IFNET_INVALID;
	struct appl_arg_if *arg_if;

	if (!classif)
		return -1;

	classif->cos_udp = build_cos_w_queue("cos_udp", group_cls);
	if (classif->cos_udp == ODP_COS_INVALID) {
		NFP_ERR("Failed to create UDP COS");
		return -1;
	}

	for (i = 0; i < itf_param->if_count; i++) {
		arg_if = &itf_param->if_array[i];

		ifnet = nfp_ifport_net_ifnet_get_by_name(arg_if->if_name);
		if (ifnet == NFP_IFNET_INVALID) {
			NFP_ERR("Failed to get NFP interface %s\n",
				arg_if->if_name);
			return -1;
		}

		if (nfp_ifnet_port_get(ifnet, &port, NULL)) {
			NFP_ERR("Failed to get NFP interface port ID%s\n",
				arg_if->if_name);
			return -1;
		}

		pktio = nfp_ifport_net_pktio_get(port);
		if (pktio == ODP_PKTIO_INVALID) {
			NFP_ERR("Failed to get pktio for interface %s",
				arg_if->if_name);
			return -1;
		}

		sprintf(name, "cos_default_%s", arg_if->if_name);
		classif->cos_def[i] = build_cos_w_queue(name, group_workers);
		if (classif->cos_def[i] == ODP_COS_INVALID) {
			NFP_ERR("Failed to create default COS "
				"for interface %s", arg_if->if_name);
			return -1;
		}

		if (odp_pktio_default_cos_set(pktio, classif->cos_def[i]) < 0) {
			NFP_ERR("Failed to set default COS on interface %s",
				arg_if->if_name);
			return -1;
		}

		if (odp_pktio_error_cos_set(pktio, classif->cos_def[i]) < 0) {
			NFP_ERR("Failed to set error COS on interface %s",
				arg_if->if_name);
			return -1;
		}

		pmr_udp = build_udp_prm(classif->cos_def[i], classif->cos_udp);
		if (pmr_udp == ODP_PMR_INVALID) {
			NFP_ERR("Failed to create UDP PRM");
			return -1;
		}
	}

	return 0;
}

int cleanup_classifier(classif_t *classif)
{
	int i;
	odp_queue_t cos_q;

	for (i = 0; i < NFP_FP_INTERFACE_MAX; i++)
		if (classif->cos_def[i] != ODP_COS_INVALID) {
			cos_q = odp_cos_queue(classif->cos_def[i]);

			if (cos_q != ODP_QUEUE_INVALID)
				odp_queue_destroy(cos_q);

			odp_cos_destroy(classif->cos_def[i]);
			classif->cos_def[i] = ODP_COS_INVALID;
		}

	if (classif->cos_udp != ODP_COS_INVALID) {
		if (classif->pmr_udp != ODP_PMR_INVALID)
			odp_cls_pmr_destroy(classif->pmr_udp);

		cos_q = odp_cos_queue(classif->cos_udp);

		if (cos_q != ODP_QUEUE_INVALID)
			odp_queue_destroy(cos_q);

		odp_cos_destroy(classif->cos_udp);
		classif->cos_udp = ODP_COS_INVALID;
	}

	return 0;
}

static odp_cos_t build_cos_w_queue(const char *name,
				   odp_schedule_group_t group_cls)
{
	odp_cos_t cos;
	odp_queue_t queue_cos;
	odp_queue_param_t qparam;
	odp_cls_cos_param_t cos_param;

	odp_queue_param_init(&qparam);
	qparam.type = ODP_QUEUE_TYPE_SCHED;
	qparam.sched.prio  = odp_schedule_default_prio();
	qparam.sched.sync  = ODP_SCHED_SYNC_ATOMIC;
	qparam.sched.group = group_cls;

	queue_cos = odp_queue_create(name, &qparam);
	if (queue_cos == ODP_QUEUE_INVALID) {
		NFP_ERR("Failed to create queue\n");
		return ODP_COS_INVALID;
	}

	odp_cls_cos_param_init(&cos_param);
	cos_param.num_queue = 1;
	cos_param.queue = queue_cos;
	cos_param.pool = odp_pool_lookup(SHM_PKT_POOL_NAME);
	cos = odp_cls_cos_create(name, &cos_param);
	if (cos == ODP_COS_INVALID) {
		NFP_ERR("Failed to create COS");
		odp_cos_destroy(cos);
		odp_queue_destroy(queue_cos);
		return ODP_COS_INVALID;
	}

	return cos;
}

static odp_pmr_t build_udp_prm(odp_cos_t cos_src, odp_cos_t cos_dst)
{
	odp_pmr_param_t pmr_param;
	uint16_t pmr_udp_val = odp_cpu_to_be_16(TEST_PORT);
	uint16_t pmr_udp_mask = 0xffff;

	odp_cls_pmr_param_init(&pmr_param);

	pmr_param.term = ODP_PMR_UDP_DPORT;
	pmr_param.range_term = false;
	pmr_param.match.value = &pmr_udp_val;
	pmr_param.match.mask = &pmr_udp_mask;
	pmr_param.val_sz = sizeof(pmr_udp_val);

	return odp_cls_pmr_create(&pmr_param, 1, cos_src, cos_dst);
}

