/***********************************************************************************
*
* Filename:     aes-wrapper.h
*
* Description:  CCM Wrapper for Mbedtls Library 
*
**********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <string.h>

#include "aes-wrapper.h"
#include "em_device.h"
#include "mbedtls/ccm.h"

/***********************************************************************************
* CONSTANTS AND DEFINES
*/
#if defined(CRYPTO_COUNT)
  #define MBEDTLS_DEVICE_COUNT CRYPTO_COUNT
#else
  #define MBEDTLS_DEVICE_COUNT 1
#endif

#define   MAC_MAX_PACKET_SIZE   (127u)
#define   MAC_MAX_MIC_SIZE      (16u)

#define   NONCE_FIXED_SIZE      0x0D

#define   MIC_SIZE_MASK         0x03
#define   ENCRYPTION_ON_MASK    0x04

/***********************************************************************************
* LOCAL VARIABLES
*/
static mbedtls_ccm_context      mbedtls_device_ctx[MBEDTLS_DEVICE_COUNT];
static int8_t                   device_instance = 0;

/***********************************************************************************
* PUBLIC FUNCTIONS DEFINITIONS
*/
Status ccm_init(uint8_t *securityKey)
{
  Status status = 0x00;

  device_instance = 0;
  mbedtls_ccm_init(&mbedtls_device_ctx[device_instance]);
  //mbedtls_ccm_set_device_instance(&mbedtls_device_ctx[device_instance],0); -- Safe to remove

  if(securityKey != NULL)
  {
    status = mbedtls_ccm_setkey(&mbedtls_device_ctx[device_instance], MBEDTLS_CIPHER_ID_AES, (const unsigned char *)securityKey ,128);
  }

  return status;
}

Status ccm_secure(  uint8_t *buffer,
                    uint8_t nonce[AES_BLOCKSIZE],
                    uint8_t *key,
                    uint8_t hdr_len,
                    uint8_t pld_len,
                    uint8_t sec_level,
                    uint8_t aes_dir)
{
  Status  status                    = 0x00;
  uint8_t micSize                   = 0x00;
  uint8_t encryptionOn              = 0x00;

  unsigned char tempTagOut[MAC_MAX_MIC_SIZE];
  uint8_t tempDataOut[MAC_MAX_PACKET_SIZE];
  
  if(key != NULL)
  {
    mbedtls_ccm_setkey(&mbedtls_device_ctx[device_instance], MBEDTLS_CIPHER_ID_AES, (const unsigned char *)key ,128);
  }

  micSize = (MIC_SIZE_MASK & sec_level) << 2;
  encryptionOn = (ENCRYPTION_ON_MASK & sec_level) >> 2;

  if(encryptionOn && (0 == pld_len))
  {
    return 0xFF;//Encryption is activated by security level but no payload is defined to be encrypted / decrypted
  }

  //First check if we encrypt or decrypt
  if(AES_DIR_ENCRYPT == aes_dir)
  {
    // skip the length byte and MAC FC and start from the next byte in TXBUF
    status = (uint8_t)mbedtls_ccm_encrypt_and_tag(  &mbedtls_device_ctx[device_instance],//mbedtls device instance (mapped to EFR32 crytpo block)
                                                    pld_len,//encrypted payload length. If 0 no encryption is needed
                                                    (const unsigned char *)(&nonce[1]),//Get 13 bytes from 16 bytes passed (starting at index = 1)
                                                    NONCE_FIXED_SIZE,//Nonce fixed size to 13 bytes (per spec),
                                                    (const unsigned char *)buffer,//additionnal data to authenticat, here header data
                                                    hdr_len,//length of additionnal data to authenticat, here header data
                                                    &buffer[hdr_len],//input data
                                                    tempDataOut,//temporary output buffer as ours is inout
                                                    tempTagOut,
                                                    micSize);

    //if security level asks for encryption, we replace the unencrypted payload in the buffer by the encrypted one
    if((0x00 == status) && encryptionOn)
    {
      memcpy(&buffer[hdr_len], tempDataOut, pld_len);
    }

    //if security level asks for MIC, we append the computed MIC at the end of the encrypted/non encrypted payload
    if(0x00 != micSize)
    {
      memcpy(&buffer[hdr_len + pld_len], tempTagOut, micSize);
    }

  } else  if(AES_DIR_DECRYPT == aes_dir)
  {
    status = (uint8_t)mbedtls_ccm_auth_decrypt( &mbedtls_device_ctx[device_instance],//mbedtls device instance (mapped to EFR32 crytpo block)
                                                pld_len,//encrypted payload length. If 0 no encryption is needed
                                                (const unsigned char *)(&nonce[1]),//Get 13 bytes from 16 bytes passed (starting at index = 1)
                                                NONCE_FIXED_SIZE,//Nonce fixed size to 13 bytes (per spec)
                                                (const unsigned char *)buffer,//additionnal data to authenticat, here header data
                                                hdr_len,//length of additionnal data to authenticat, here header data
                                                &buffer[hdr_len],//input data
                                                tempDataOut,//temporary output buffer as ours is inout
                                                (const unsigned char *)(&buffer[hdr_len+pld_len]),//MIC ot Tag buffer. Appended at end of packet
                                                micSize);//MIC or Tag buffer length

    //if security level asks for encryption, we replace the encrypted payload in the buffer by the decrypted one
    if((0x00 == status) && encryptionOn)
    {
      memcpy(&buffer[hdr_len], tempDataOut, pld_len);
    }

  } else 
  {
      //To be defined by Signify Pro Team
  }

  return status;
}
