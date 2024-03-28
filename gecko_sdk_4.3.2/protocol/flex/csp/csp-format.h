/***************************************************************************//**
 * @brief Functions for seralization and de-serialization used with RTOS or NCP
 * features
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __CSP_FORMAT_H__
#define __CSP_FORMAT_H__

// These 2 defines should be generated by actually setting them to the maximum
// needed size for both APIs and stack callbacks. For now they are set to some
// large value.
#define MAX_STACK_API_COMMAND_SIZE                      170
#define MAX_STACK_CALLBACK_COMMAND_SIZE                 170

/**
 * Extract data from the buffer into va_list according to the format
 */
void fetchParams(uint8_t *readPointer, PGM_P format, va_list args);

uint8_t computeCommandLength(uint8_t initialLength, const char* format, va_list argumentList);

/**
 * Extract data from the buffer into the argument list according to the format
 * Call fetchParams after initializing a va_list.
 * apiCommandData begins with a command name on 16bits, so fetchParams is called
 * with apiCommandData + 2
 */
void fetchApiParams(uint8_t *apiCommandData, PGM_P format, ...);

/**
 * Extract data from the buffer into the argument list according to the format
 * Call fetchParams after initializing a va_list
 */
void fetchCallbackParams(uint8_t *callbackParams, PGM_P format, ...);

/**
 * Format a buffer to be sent through CSP from an argument list according to the format.
 * Fill the buffer according to the format and the given va_list
 */
uint16_t formatResponseCommandFromArgList(uint8_t *buffer,
                                          uint16_t bufferSize,
                                          uint16_t identifier,
                                          const char *format,
                                          va_list argumentList);

/**
 * Format a buffer to be sent through CSP from an argument list according to the format.
 * Initialize a va_list from the given format and call formatResponseCommandFromArgList()
 */
uint16_t formatResponseCommand(uint8_t *buffer,
                               uint16_t bufferSize,
                               uint16_t identifier,
                               PGM_P format,
                               ...);

#endif
