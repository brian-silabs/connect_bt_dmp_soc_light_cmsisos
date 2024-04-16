/***************************************************************************//**
 * @file
 * @brief app_init.c
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
#include "app_log.h"
#include "stack/include/ember.h"
#include "app_process.h"
#include "app_framework_common.h"
#include "sl_light_switch.h"
// Ensure that psa is initialized corretly
#include "psa/crypto.h"

#include "aes-wrapper.h"

#include "sl_bt_api.h"
#include "em_gpio.h"

Status ccmStatus = 0x00;

// Current security key : default init with GP group key default
uint8_t key[]= {
    0x70, 0xAD, 0x7D, 0x16, 0xDD, 0xDF, 0x0C, 0x04,
    0x3B, 0x08, 0x5F, 0x7D, 0x33, 0x53, 0x2B, 0x44,
};

uint8_t originalData[127] = {
    0xff,0x48,0x1,0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x17,0x88,
    0x1,0x0,0x80,0x73,0x9e,0x0,0x11,0x3b,0x72,0x0,0x97,
    0xb0,0x6f,0xd3,0xda,0x1c,0xe,0x30,0x6d,0x91,0xc4,
    //0x50,0x45,0xfe,0x79// expected MIC
};

uint8_t initVector[16] = {
    0x00,  0x93,  0x00,  0x00,  0x00,  0x01,  0xff,  0xff,
    0xff,  0xff,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00
};


// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The event handler signal of the state machine
extern EmberEventControl *state_machine_event;
///This structure contains all the flags used in the state machine
extern light_application_flags_t state_machine_flags;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The function is used for some basic initialization relates to the app.
 *****************************************************************************/
void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your application init code here!                                    //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  GPIO_PinModeSet(gpioPortB, 0, gpioModePushPull, 1);
  sl_bt_system_start_bluetooth();
}

/******************************************************************************
* Application framework init callback
******************************************************************************/
void emberAfInitCallback(void)
{
  // Ensure that psa is initialized corretly
  psa_crypto_init();

  emberAfAllocateEvent(&state_machine_event, &state_machine_handler);
  emberEventControlSetDelayMS(*state_machine_event, 100);
  // CLI info message
  app_log_info("\nLight DMP\n");

  // set the default PAN ID, it can be changed with CLI
  sl_set_pan_id(DEFAULT_LIGHT_SWITCH_PAN_ID);
  // set the default communication channel, it can be changed with CLI
  app_log_info("Deafult channel> %d\n", emberGetDefaultChannel());
  sl_set_channel(emberGetDefaultChannel());

  emberNetworkInit();
  state_machine_flags.init_success = true;

#if defined(EMBER_AF_PLUGIN_BLE)
  bleConnectionInfoTableInit();
#endif

  //Initialization of Security Library using key
  app_log_info("Security Init\n");
  ccm_init(key);

  //First authenticate only our test vector
  app_log_info("Starting test vector encr authentication\n");
  ccmStatus = ccm_secure(originalData, initVector, NULL, 33, 0, 0x01, AES_DIR_ENCRYPT);
  app_log_info("CCM authentication vector test status: 0x%X\n", ccmStatus);
  if(!ccmStatus)
  {
    app_log_info("Generated MIC is 0x%4x LSByte first\n", *((uint32_t *)(&originalData[33])));
  }

  app_log_info("Starting test vector decr authentication\n");
  ccmStatus = ccm_secure(originalData, initVector, NULL, 33, 0, 0x01, AES_DIR_DECRYPT);
  app_log_info("CCM vector test status: 0x%x\n", ccmStatus);

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
