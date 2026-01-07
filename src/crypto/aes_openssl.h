#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// AES-256-CBC using OpenSSL EVP. Output: [IV(16)] [CIPHERTEXT(...)] with PKCS#7 handled by EVP.
// Derives 32-byte key from passphrase with SHA-256 (educational; in real life use PBKDF2/Argon2).

int aes256_cbc_encrypt_passphrase(const uint8_t* plaintext, size_t pt_len,
                                  const char* passphrase,
                                  uint8_t** out, size_t* out_len);

int aes256_cbc_decrypt_passphrase(const uint8_t* in, size_t in_len,
                                  const char* passphrase,
                                  uint8_t** plaintext, size_t* pt_len);

#ifdef __cplusplus
}
#endif
