/***************************************************************************//**
 * @file
 * @brief app_cli.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include PLATFORM_HEADER
#include "em_chip.h"
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "sl_cli.h"
#include "app_log.h"
#include "app_process.h"
#include "sl_light_switch.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define ENABLED  "enabled"
#define DISABLED "disabled"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
// Connect Tx options
extern volatile EmberMessageOptions tx_options;
/// Connect security key id
extern psa_key_id_t security_key_id;
/// Indicates when light toggle action is required
extern bool light_toggle_required;
/// Destination of the currently processed sink node, defaults to Coordinator
extern EmberNodeId light_node_id;
/// In the starting state, the switch tries to connect to a network
extern light_switch_state_machine_t state;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
///This structure contains all the flags used in the state machine
extern light_application_flags_t state_machine_flags;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

/******************************************************************************
 * CLI - Join a local network as MAC Mode device
 *****************************************************************************/
void cli_join_commissioned(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  state_machine_flags.join_commissioned_network_request = true;
}

/******************************************************************************
 * CLI - Join a local network
 *****************************************************************************/
void cli_join(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  state_machine_flags.join_network_request = true;
}


/******************************************************************************
 * CLI - Form a local network
 *****************************************************************************/
void cli_form(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  state_machine_flags.form_network_request = true;
}

/******************************************************************************
 * CLI - leave command
 *****************************************************************************/
void cli_leave(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  emberResetNetworkState();
  state_machine_flags.leave_request = true;
}

/******************************************************************************
 * CLI - reset command
 *****************************************************************************/
void cli_reset(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  halReboot();
}

/******************************************************************************
 * CLI - info command
 * It lists the main attributes of the current state of the node
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  char* is_ack = ((tx_options & EMBER_OPTIONS_ACK_REQUESTED) ? ENABLED : DISABLED);
  char* is_security = ((tx_options & EMBER_OPTIONS_SECURITY_ENABLED) ? ENABLED : DISABLED);
  char* is_high_prio = ((tx_options & EMBER_OPTIONS_HIGH_PRIORITY) ? ENABLED : DISABLED);

  app_log_info("Info:\n");
  app_log_info("         MCU Id: 0x%016llX\n", SYSTEM_GetUnique());
  app_log_info("  Network state: 0x%02X\n", emberNetworkState());
  app_log_info("      Node type: 0x%02X\n", emberGetNodeType());
  app_log_info("        Node id: 0x%04X\n", emberGetNodeId());
  app_log_info("         Pan id: 0x%04X\n", emberGetPanId());
  app_log_info("        Channel: %d\n", (uint16_t)emberGetRadioChannel());
  app_log_info("          Power: %d\n", (int16_t)emberGetRadioPower());
  app_log_info("     TX options: MAC acks %s, security %s, priority %s\n", is_ack, is_security, is_high_prio);
}

/******************************************************************************
 * CLI - set PAN ID
 *****************************************************************************/
void cli_set_pan_id(sl_cli_command_arg_t *arguments)
{
  uint16_t pan_id = sl_cli_get_argument_uint16(arguments, 0);
  sl_set_pan_id(pan_id);
}

/******************************************************************************
 * CLI - set the communication channel
 *****************************************************************************/
void cli_set_channel(sl_cli_command_arg_t *arguments)
{
  uint16_t channel = sl_cli_get_argument_uint16(arguments, 0);
  sl_set_channel(channel);
}

/******************************************************************************
 * CLI - set_tx_options command
 * Sets the tx options
 *****************************************************************************/
void cli_set_tx_option(sl_cli_command_arg_t *arguments)
{
  tx_options = sl_cli_get_argument_uint8(arguments, 0);
  char* is_ack = ((tx_options & EMBER_OPTIONS_ACK_REQUESTED) ? ENABLED : DISABLED);
  char* is_security = ((tx_options & EMBER_OPTIONS_SECURITY_ENABLED) ? ENABLED : DISABLED);
  char* is_high_prio = ((tx_options & EMBER_OPTIONS_HIGH_PRIORITY) ? ENABLED : DISABLED);

  app_log_info("TX options set: MAC acks %s, security %s, priority %s\n", is_ack, is_security, is_high_prio);
}

/******************************************************************************
 * CLI - set_security_key command
 * The command sets the security key
 *****************************************************************************/
void cli_set_security_key(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
  uint8_t *key_hex_value = 0;
  size_t key_hex_length = 0;
  key_hex_value = sl_cli_get_argument_hex(arguments, 0, &key_hex_length);
  if (key_hex_length != EMBER_ENCRYPTION_KEY_SIZE) {
    app_log_info("Security key length must be: %d bytes\n", EMBER_ENCRYPTION_KEY_SIZE);
    return;
  }

  set_security_key(key_hex_value, key_hex_length);

#else
  (void)arguments;
  app_log_info("Security plugin: CONNECT AES SECURITY is missing\n");
  app_log_info("Security key set failed 0x%02X\n", EMBER_ERR_FATAL);
#endif
}

/******************************************************************************
 * CLI - unset_security_key command
 * The command deletes the security key
 *****************************************************************************/
void cli_unset_security_key(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  #ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
  if (security_key_id != 0) {
    psa_destroy_key(security_key_id);
  }
  emberRemovePsaSecurityKey();
  security_key_id = 0;
  app_log_info("Security key unset successful\n");
  #endif
}

/******************************************************************************
 * CLI - Light toggle command
 *****************************************************************************/
void cli_toggle_light(sl_cli_command_arg_t *arguments)
{
  uint16_t destinationShortId = sl_cli_get_argument_uint16(arguments, 0);
  if (state == S_OPERATE) {
    light_node_id = destinationShortId;
    light_toggle_required = true;
  }
}

/******************************************************************************
 * CLI - send message: Send message
 * Params:
 * 0: a "nibble mask" indicating which fields are specified, specifically:
 *    0x000F - source ID mode (0x00 = none, 0x02 = short, 0x03 = long)
 *    0x00F0 - destination ID mode (0x00 = none, 0x02 = short, 0x03 = long)
 *    0x0F00 - the source pan ID is specified (0x01) or not (0x00).
 *    0xF000 - the destination pan ID is specified (0x01) or not (0x00).
 * 1: the source short ID (if specified)
 * 2: the source long ID (if specified)
 * 3: the destination short ID (if specified)
 * 4: the destination long ID (if specified)
 * 5: the source PAN ID (if specified)
 * 6: the destination PAN ID (if specified)
 * 7: MAC payload length
 *****************************************************************************/
void cli_send(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberMacFrame mac_frame;
  size_t length;
  uint16_t mac_frame_info = sl_cli_get_argument_uint16(arguments, 0);
  EmberNodeId short_src_id = sl_cli_get_argument_uint16(arguments, 1);
  EmberNodeId short_dest_id = sl_cli_get_argument_uint16(arguments, 3);
  EmberPanId src_pan_id = sl_cli_get_argument_uint16(arguments, 5);
  EmberPanId dst_pan_id = sl_cli_get_argument_uint16(arguments, 6);
  uint8_t *message = sl_cli_get_argument_hex(arguments, 7, &length);

  if ((mac_frame_info & 0x000F) == EMBER_MAC_ADDRESS_MODE_SHORT) {
    mac_frame.srcAddress.addr.shortAddress = short_src_id;
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_SHORT;
  } else if ((mac_frame_info & 0x000F) == EMBER_MAC_ADDRESS_MODE_LONG) {
    uint8_t *hex_value = 0;
    size_t hex_length = 0;
    hex_value = sl_cli_get_argument_hex(arguments, 2, &hex_length);
    if (EUI64_SIZE != hex_length) {
      app_log_error("Source MAC address set failed, not correct length\n");
      return;
    }
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      mac_frame.srcAddress.addr.longAddress[i] = hex_value[i];
    }
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_LONG;
  } else {
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_NONE;
  }

  if (((mac_frame_info & 0x00F0) >> 4) == EMBER_MAC_ADDRESS_MODE_SHORT) {
    mac_frame.dstAddress.addr.shortAddress = short_dest_id;
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_SHORT;
  } else if (((mac_frame_info & 0x00F0) >> 4) == EMBER_MAC_ADDRESS_MODE_LONG) {
    uint8_t *hex_value = 0;
    size_t hex_length = 0;
    hex_value = sl_cli_get_argument_hex(arguments, 4, &hex_length);
    if (EUI64_SIZE != hex_length) {
      app_log_error("Destination MAC address set failed, not correct length\n");
      return;
    }
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      mac_frame.dstAddress.addr.longAddress[i] = hex_value[i];
    }
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_LONG;
  } else {
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_NONE;
  }

  if (mac_frame_info & 0x0F00) {
    mac_frame.srcPanId = src_pan_id;
    mac_frame.srcPanIdSpecified = true;
  } else {
    mac_frame.srcPanIdSpecified = false;
  }

  if (mac_frame_info & 0xF000) {
    mac_frame.dstPanId = dst_pan_id;
    mac_frame.dstPanIdSpecified = true;
  } else {
    mac_frame.dstPanIdSpecified = false;
  }

  status = emberMacMessageSend(&mac_frame,
                               0x00, // messageTag
                               length,
                               message,
                               tx_options);

  if (status == EMBER_SUCCESS) {
    app_log_info("MAC frame submitted\n");
  } else {
    app_log_error("MAC frame submission failed, status=0x%02X\n",
                  status);
  }
}
