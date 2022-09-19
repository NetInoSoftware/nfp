/* Copyright (c) 2014, ENEA Software AB
 * Copyright (c) 2014, Nokia
 * All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */

#ifndef __NFP_API_CLI_H__
#define __NFP_API_CLI_H__

#include <odp_api.h>
#include "nfp_init.h"
#include "nfp_socket.h"

#if __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

/**
 * Start CLI thread.
 *
 * Called by Application code to start the CLI thread that uses
 * OS socket API.
 *
 * @param core_id The core on which the CLI server thread is started.
 * Use NFP_CONTROL_CORE to start the thread on the 'linux_core_id'
 * configured at NFP initialization time.
 * Use NFP_DFLT_CLI_CORE to start the thread on the
 * 'cli.os_thread.core_id' configured at NFP initialization time.
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */

int nfp_cli_start_os_thread(int core_id);

/**
 * Stop CLI server thread that uses OS socket API.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */
int nfp_cli_stop_os_thread(void);

/**
 * Start NFP CLI thread.
 *
 * Called by Application code to start the CLI thread that uses
 * NFP socket API.
 *
 * @param core_id The core on which the CLI server thread is started.
 * Use NFP_CONTROL_CORE to start the thread on the 'linux_core_id'
 * configured at NFP initialization time.
 * Use NFP_DFLT_CLI_CORE to start the thread on the
 * 'cli.nfp_thread.core_id' configured at NFP initialization time.
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */
int nfp_cli_start_nfp_thread(int core_id);

/**
 * Stop CLI server thread that uses NFP socket API.
 *
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */
int nfp_cli_stop_nfp_thread(void);

/**
 * @page customCliCmd Customized CLI commands.
 *
 * CLI commands have the format
 * <pre>
 * keyword [keyword | arg]...
 * </pre>
 * where keyword is any string and arg is a placeholder for
 * one of the following:
 *
 * <pre>
 * Arg      Format
 * ---      ------
 * NUMBER   number
 * IP4ADDR  a.b.c.d
 * STRING   string
 * DEV      device name
 * IP4NET   a.b.c.d/n
 * IP6ADDR  a:b:c:d:e:f:g:h"
 * IP6NET   a:b:c:d:e:f:g:h/n"
 * </pre>
 *
 * Example: Add an IP address to an array position:
 *
 * @code
 * void my_func(void *handle, const char *args)
 * {
 *     int pos;
 *     uint32_t addr;
 *     // args has format "10.20.30.4 5"
 *     [...parse args...]
 *
 *     if (my_array[pos] == 0) {
 *         nfp_cli_print(handle, "Pos not free!\r\n", 15);
 *         return;
 *     }
 *
 *     my_array[pos] = addr;
 *     nfp_cli_print(handle, "OK\r\n", 4, 0);
 * }
 *
 * nfp_cli_add_command("add_ip_addr IP4ADDR to NUMBER",
 *                     "Add an IP address to a table position"
 *                     my_func);
 * @endcode
 *
 * Valid CLI command would be for example:
 * <pre>
 * add_ip_addr 10.20.30.4 to 5
 * </pre>
 */

/**
 * CLI callback function type.
 *
 * @param  handle  The handle used to print the command output.
 * @param  args    Command line arguments separated by a space.
 */
typedef void (*nfp_cli_cb_func)(void *handle, const char *args);

/**
 * Add a new CLI command. See @ref customCliCmd "customized CLI commands documentation".
 *
 * @param  cmd   Command line.
 * @param  help  Help text for the command.
 * @param  func  Function to call when CLI command is executed.
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */
int nfp_cli_add_command(const char *cmd, const char *help,
			nfp_cli_cb_func func);

/**
 * Print the output of the CLI command
 *
 * @param  handle   The CLI command function handle argument
 * @param  buf      Buffer containing the text to print
 * @param  buf_size Size of the buffer
 * @retval Number of characters printed.
 * @retval <0 Failure.
 */
int nfp_cli_print(void *handle, char *buf, nfp_size_t buf_size);

/**
 * Process the CLI commands from the file
 *
 * @param cli_file Name of the CLI file to process.
 * @retval 0 Success.
 * @retval -1 Failure.
 * @retval NFP_ENOTSUP NFP has been compiled without CLI support or is disabled.
 */
int nfp_cli_process_file(char *cli_file);
#if __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#endif /* __NFP_API_CLI_H__ */
