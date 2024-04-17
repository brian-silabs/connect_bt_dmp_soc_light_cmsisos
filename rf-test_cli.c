
#include "sl_cli.h"
#include "sl_cli_config.h"
#include "sl_cli_command.h"
#include "sl_cli_arguments.h"
#include "sl_cli_config_example.h"

#include "app_log.h"
#include "rail.h"

void setCtune(sl_cli_command_arg_t *arguments);
void getCtune(sl_cli_command_arg_t *arguments);

// Create command details for the commands. The macro SL_CLI_UNIT_SEPARATOR can be
// used to format the help text for multiple arguments.
static const sl_cli_command_info_t cli_cmd__setCtune = \
  SL_CLI_COMMAND(setCtune,
                 "Set the value of HFXO CTUNE. The radio must be IDLE.",
                  "ctune" SL_CLI_UNIT_SEPARATOR,
                 {SL_CLI_ARG_UINT16, SL_CLI_ARG_END, });

static const sl_cli_command_info_t cli_cmd__getCtune = \
  SL_CLI_COMMAND(getCtune,
                 "Get the value of HFXO CTUNE",
                  "",
                 {SL_CLI_ARG_END, });

// Create the array of commands, containing three elements in this example
static sl_cli_command_entry_t rf_test_cli_table[] = {
  { "setCtune", &cli_cmd__setCtune, false },
  { "getCtune", &cli_cmd__getCtune, false },
  { NULL, NULL, false },
};

// Create the command group at the top level
static sl_cli_command_group_t a_group_0 = {
  { NULL },
  false,
  rf_test_cli_table
};

// Create command handlers for the commands
void setCtune(sl_cli_command_arg_t *arguments)
{
  uint16_t argument_value = sl_cli_get_argument_uint16(arguments, 0);
  RAIL_Status_t status = RAIL_SetTune(RAIL_EFR32_HANDLE, argument_value);
  app_log_info("RAIL_SetTune returned: 0x%02X\n", status);
}

void getCtune(sl_cli_command_arg_t *arguments)
{
  uint16_t currentCtune = RAIL_GetTune(RAIL_EFR32_HANDLE);
  app_log_info("Current CTUNE: 0x%02X\n", currentCtune);
}

sl_status_t rfTestCliInit(void)
{
  sl_status_t status = sl_cli_command_add_command_group(sl_cli_default_handle, &a_group_0);
  return status;
}
