#ifndef CIPHER_H
#define CIPHER_H

#include <stdint.h>
#include <stddef.h>

#define BLOCK_SIZE 16  // 128-bit block size
#define KEY_SIZE 32    // 256-bit key size
#define IV_SIZE 16     // 128-bit IV size

// Authorial symmetric cipher - block encryption/decryption
void cipher_encrypt_block(const uint8_t *input, uint8_t *output, const uint8_t *key);
void cipher_decrypt_block(const uint8_t *input, uint8_t *output, const uint8_t *key);

// CBC mode operations
int cbc_encrypt(const uint8_t *plaintext, size_t plaintext_len,
                const uint8_t *key, const uint8_t *iv,
                uint8_t **ciphertext, size_t *ciphertext_len);

int cbc_decrypt(const uint8_t *ciphertext, size_t ciphertext_len,
                const uint8_t *key, const uint8_t *iv,
                uint8_t **plaintext, size_t *plaintext_len);

#endif // CIPHER_H
