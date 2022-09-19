/* Copyright (c) 2014, ENEA Software AB
 * Copyrighy (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */
#include "nfp.h"

// Test for successful compile & link.
int main() {
	static nfp_initialize_param_t oig;

	nfp_initialize_param(&oig);

	if (nfp_initialize(&oig)) {
		NFP_ERR("Error: NFP global init failed.\n");
		exit(EXIT_FAILURE);
	}

	NFP_INFO("Init successful.\n");

	nfp_terminate();
	return 0;
}
