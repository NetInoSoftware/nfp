/* Copyright (c) 2021, Bogdan Pricope
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_EXAMPLE_CLI_ARG_PARSE__
#define __NFP_EXAMPLE_CLI_ARG_PARSE__

typedef struct {
	int if_count;		/**< Number of interfaces to be used */
	struct appl_arg_if {
		char *if_name;
		char *if_address;
		int if_address_masklen;
	} *if_array;		/**< Array of interface parameters */
} appl_arg_ifs_t;

int nfpexpl_parse_interfaces(char *argv, appl_arg_ifs_t *itf_param);
void nfpexpl_parse_interfaces_param_cleanup(appl_arg_ifs_t *itf_param);

#endif /* __NFP_EXAMPLE_CLI_ARG_PARSE__ */
