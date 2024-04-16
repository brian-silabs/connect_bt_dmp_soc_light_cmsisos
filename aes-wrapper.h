/***********************************************************************************
 *
 * Filename:     aes-wrapper.h
 *
 * Description:  CCM Wrapper for Mbedtls Library 
 *
 **********************************************************************************/

#ifndef _AES_WRAPPER_H_
#define _AES_WRAPPER_H_

/***********************************************************************************
* INCLUDES
*/
#include <stdint.h>

/***********************************************************************************
* TEMPORARY TYPEDEFS, DEFINES & ENUMS
*/
#define     AES_BLOCKSIZE     16
typedef     int32_t           Status;
enum AesDirs
{
  AES_DIR_ENCRYPT =0,
  AES_DIR_DECRYPT =1
};

/***********************************************************************************
* PUBLIC FUNCTIONS DECLARATIONS
*/

/*
* This functions Initializes the HW cryptographic block and the mbedtls library
*
* @param       uint8_t * securityKey : pointer to security key to register
*/
Status ccm_init(uint8_t *securityKey);

/*
* This functions secures one block with CCM* according to 802.15.4.
*
* @param[in,out] buffer Input: plaintext header and payload concatenated;
*                       for encryption: MUST HAVE 'AES_BLOCKSIZE'
*                       BYTES SPACE AT THE END FOR THE MIC!
*                       Output: frame secured (with MIC at end)/unsecured
* @param[in]  nonce   The nonce: Initialization Vector (IV) as used in
*                     cryptography; the ZigBee nonce (13 bytes long)
*                     are the bytes 2...14 of this nonce
* @param[in] key The key to be used; if NULL, use the current key
* @param[in] hdr_len Length of plaintext header (will not be encrypted)
* @param[in] pld_len Length of payload to be encrypted; if 0, then only MIC
*                    authentication implies 
*                    ???? and hdr_len = hdr_len + pld_len ???? wrapper developped assuming this
* @param[in] sec_level Security level according to IEEE 802.15.4,
*                    7.6.2.2.1, Table 95:
*                    - the value may be 0 ... 7;
*                    - the two LSBs contain the MIC length in bytes
*                      (0, 4, 8 or 16);
*                    - bit 2 indicates whether encryption applies or not
* @param[in] aes_dir AES_DIR_ENCRYPT if secure, AES_DIR_DECRYPT if unsecure
*
* @return CCM Status
*/
Status ccm_secure(  uint8_t *buffer,
                    uint8_t nonce[AES_BLOCKSIZE],
                    uint8_t *key,
                    uint8_t hdr_len,
                    uint8_t pld_len,
                    uint8_t sec_level,
                    uint8_t aes_dir);

#endif //_AES_WRAPPER_H_
