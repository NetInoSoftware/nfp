/* Copyright (c) 2022, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_CLASSIFIER_H__
#define __NFP_CLASSIFIER_H__

#include "nfp.h"
#include "odp_api.h"
#include "cli_arg_parse.h"

#define TEST_PORT 54321

typedef struct {
	odp_cos_t cos_udp;
	odp_pmr_t pmr_udp;
	odp_cos_t cos_def[NFP_FP_INTERFACE_MAX];
} classif_t;

void init_classifier(classif_t *classif);
int build_classifier(appl_arg_ifs_t *itf_param,
		     odp_schedule_group_t group_cls,
		     odp_schedule_group_t group_workers,
		     classif_t *classif);
int cleanup_classifier(classif_t *classif);

#endif /* __NFP_CLASSIFIER_H__ */
