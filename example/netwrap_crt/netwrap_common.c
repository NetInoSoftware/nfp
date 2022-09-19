/* Copyright (c) 2020.
 * All rights reserved.
 *
 * SPDX-License-Identifier:     BSD-3-Clause
 */

#include "netwrap_common.h"

nfp_initialize_param_t nfp_global_params = {0};
odp_bool_t netwrap_constructor_called;      /* = 0 */

int setup_common_vars(void)
{
	nfp_param_t param = {0};

	if (nfp_get_parameters(&param))
		return -1;

	nfp_global_params = param.global_param;
	netwrap_constructor_called = 1;

	return 0;
}
