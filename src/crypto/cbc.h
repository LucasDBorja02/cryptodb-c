#pragma once
#include <stddef.h>
#include <stdint.h>
#include "crypto/xorfeistel.h"

#ifdef __cplusplus
extern "C" {
#endif

// CBC mode for XFS (educational). Uses PKCS#7 padding. Output layout: [IV(16)] [CIPHERTEXT(...)]
// If iv == NULL, generates IV with OpenSSL RAND_bytes.

int xfs_cbc_encrypt(const xfs_ctx_t* ctx,
                    const uint8_t* plaintext, size_t pt_len,
                    const uint8_t iv[XFS_BLOCK_SIZE], // optional
                    uint8_t** out, size_t* out_len);

int xfs_cbc_decrypt(const xfs_ctx_t* ctx,
                    const uint8_t* in, size_t in_len,
                    uint8_t** plaintext, size_t* pt_len);

#ifdef __cplusplus
}
#endif
