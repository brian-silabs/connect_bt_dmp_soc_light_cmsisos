/***************************************************************************//**
 * @file sl_cli_command_table.c
 * @brief Declarations of relevant command structs for cli framework.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdlib.h>

#include "sl_cli_config.h"
#include "sl_cli_command.h"
#include "sl_cli_arguments.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   TEMPLATED FILE   ******************************
 ******************************************************************************/

/*******************************************************************************
 * Example syntax (.slcc or .slcp) for populating this file:
 *
 *   template_contribution:
 *     - name: cli_command          # Register a command
 *       value:
 *         name: status             # Name of command
 *         handler: status_command  # Function to be called. Must be defined
 *         help: "Prints status"    # Optional help description
 *         shortcuts:               # Optional shorcut list
 *           - name: st
 *         argument:                # Argument list, if apliccable
 *           - type: uint8          # Variable type
 *             help: "Channel"      # Optional description
 *           - type: string
 *             help: "Text"
 *     - name: cli_group            # Register a group
 *       value:
 *         name: shell              # Group name
 *         help: "Shell commands"   # Optional help description
 *         shortcuts:               # Optional shorcuts
 *           - name: sh
 *     - name: cli_command
 *       value:
 *         name: repeat
 *         handler: repeat_cmd
 *         help: "Repeat commands"
 *         shortcuts:
 *           - name: r
 *           - name: rep
 *         group: shell            # Associate command with group
 *         argument:
 *           - type: string
 *             help: "Text"
 *           - type: additional
 *             help: "More text"
 *
 * For subgroups, an optional unique id can be used to allow a particular name to
 * be used more than once. In the following case, from the command line the
 * following commands are available:
 *
 * >  root_1 shell status
 * >  root_2 shell status
 *
 *     - name: cli_group            # Register a group
 *       value:
 *         name: root_1             # Group name
 *
 *     - name: cli_group            # Register a group
 *       value:
 *         name: root_2             # Group name
 *
 *    - name: cli_group             # Register a group
 *       value:
 *         name: shell              # Group name
 *         id: shell_root_1         # Optional unique id for group
 *         group: root_1            # Add group to root_1 group
 *
 *    - name: cli_group             # Register a group
 *       value:
 *         name: shell              # Group name
 *         id: shell_root_2         # Optional unique id for group
 *         group: root_2            # Add group to root_1 group
 *
 *    - name: cli_command           # Register a command
 *       value:
 *         name: status
 *         handler: status_1
 *         group: shell_root_1      # id of subgroup
 *
 *    - name: cli_command           # Register a command
 *       value:
 *         name: status
 *         handler: status_2
 *         group: shell_root_2      # id of subgroup
 *
 ******************************************************************************/

// Provide function declarations
void connect_ble_cli_hello_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_get_address_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_set_adv_params_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_start_adv_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_open_connection_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_close_connection_command(sl_cli_command_arg_t *arguments);
void connect_ble_cli_set_connection_params_command(sl_cli_command_arg_t *arguments);
void cli_form(sl_cli_command_arg_t *arguments);
void cli_join(sl_cli_command_arg_t *arguments);
void cli_leave(sl_cli_command_arg_t *arguments);
void cli_reset(sl_cli_command_arg_t *arguments);
void cli_info(sl_cli_command_arg_t *arguments);
void cli_set_pan_id(sl_cli_command_arg_t *arguments);
void cli_set_channel(sl_cli_command_arg_t *arguments);
void cli_set_tx_option(sl_cli_command_arg_t *arguments);
void cli_set_security_key(sl_cli_command_arg_t *arguments);
void cli_unset_security_key(sl_cli_command_arg_t *arguments);
void cli_toggle_light(sl_cli_command_arg_t *arguments);

// Command structs. Names are in the format : cli_cmd_{command group name}_{command name}
// In order to support hyphen in command and group name, every occurence of it while
// building struct names will be replaced by "_hyphen_"
static const sl_cli_command_info_t cli_cmd__ble_hello = \
  SL_CLI_COMMAND(connect_ble_cli_hello_command,
                 "Issue an Hello command to the BLE stack",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_get_address = \
  SL_CLI_COMMAND(connect_ble_cli_get_address_command,
                 "Get the BLE address",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_set_adv_params = \
  SL_CLI_COMMAND(connect_ble_cli_set_adv_params_command,
                 "Set the BLE advertisement parameters",
                  "Minimum interval" SL_CLI_UNIT_SEPARATOR "Maximum interval" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT16, SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_start_adv = \
  SL_CLI_COMMAND(connect_ble_cli_start_adv_command,
                 "Start BLE advertising",
                  "Discoverable mode" SL_CLI_UNIT_SEPARATOR "Connectable mode" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT8, SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_open_connection = \
  SL_CLI_COMMAND(connect_ble_cli_open_connection_command,
                 "Open a BLE connection",
                  "Address" SL_CLI_UNIT_SEPARATOR "Address type" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_HEX, SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_close_connection = \
  SL_CLI_COMMAND(connect_ble_cli_close_connection_command,
                 "Close a BLE connection",
                  "Connection handle" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__ble_set_connection_params = \
  SL_CLI_COMMAND(connect_ble_cli_set_connection_params_command,
                 "Set the BLE default connection parameters",
                  "Minimum interval" SL_CLI_UNIT_SEPARATOR "Maximum interval" SL_CLI_UNIT_SEPARATOR "Slave latency" SL_CLI_UNIT_SEPARATOR "Supervision timeout" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT16, SL_CLI_ARG_UINT16, SL_CLI_ARG_UINT16, SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__form = \
  SL_CLI_COMMAND(cli_form,
                 "Forms a network with a default address",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__join = \
  SL_CLI_COMMAND(cli_join,
                 "Joins a network with a default address as Sleepy",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__leave = \
  SL_CLI_COMMAND(cli_leave,
                 "Forget the current network and revert to EMBER_NO_NETWORK",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__reset = \
  SL_CLI_COMMAND(cli_reset,
                 "Reset the hardware",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__info = \
  SL_CLI_COMMAND(cli_info,
                 "MCU ID, Network state, Node ID, PAN ID, Channel ID, etc.",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__set_pan_id = \
  SL_CLI_COMMAND(cli_set_pan_id,
                 "Set the PAN ID of the node",
                  "2 byte PAN ID" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__set_channel = \
  SL_CLI_COMMAND(cli_set_channel,
                 "Set the communication channel",
                  "2 byte channel number" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__set_tx_options = \
  SL_CLI_COMMAND(cli_set_tx_option,
                 "Set Tx options",
                  "Security(0x01) MAC Ack(0x02) High Prio(0x04)" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT8, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__set_key = \
  SL_CLI_COMMAND(cli_set_security_key,
                 "Set security key",
                  "Security key (size:EMBER_ENCRYPTION_KEY_SIZE)" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_HEX, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__unset_key = \
  SL_CLI_COMMAND(cli_unset_security_key,
                 "Unset current security key",
                  "",
                 {SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__toggle_light = \
  SL_CLI_COMMAND(cli_toggle_light,
                 "Toggle the light on the Light node",
                  "2 byte short ID of destination",
                  {SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

// Create group command tables and structs if cli_groups given
// in template. Group name is suffixed with _group_table for tables
// and group commands are cli_cmd_grp_( group name )
// Create root command table
const sl_cli_command_entry_t sl_cli_default_command_table[] = {
  { "ble_hello", &cli_cmd__ble_hello, false },
  { "ble_get_address", &cli_cmd__ble_get_address, false },
  { "ble_set_adv_params", &cli_cmd__ble_set_adv_params, false },
  { "ble_start_adv", &cli_cmd__ble_start_adv, false },
  { "ble_open_connection", &cli_cmd__ble_open_connection, false },
  { "ble_close_connection", &cli_cmd__ble_close_connection, false },
  { "ble_set_connection_params", &cli_cmd__ble_set_connection_params, false },
  { "form", &cli_cmd__form, false },
  { "join", &cli_cmd__join, false },
  { "leave", &cli_cmd__leave, false },
  { "reset", &cli_cmd__reset, false },
  { "info", &cli_cmd__info, false },
  { "set_pan_id", &cli_cmd__set_pan_id, false },
  { "set_channel", &cli_cmd__set_channel, false },
  { "set_tx_options", &cli_cmd__set_tx_options, false },
  { "set_key", &cli_cmd__set_key, false },
  { "unset_key", &cli_cmd__unset_key, false },
  { "toggle_light", &cli_cmd__toggle_light, false },
  { NULL, NULL, false },
};


#ifdef __cplusplus
}
#endif
