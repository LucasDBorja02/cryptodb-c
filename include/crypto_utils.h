#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <stdint.h>
#include <stddef.h>

// Generate random bytes for IV and key generation
int generate_random_bytes(uint8_t *buffer, size_t length);

// PKCS7 padding functions
size_t add_pkcs7_padding(uint8_t *data, size_t data_len, size_t block_size);
int remove_pkcs7_padding(uint8_t *data, size_t data_len, size_t *unpadded_len);

// Utility functions for hex conversion
void bytes_to_hex(const uint8_t *bytes, size_t len, char *hex_str);
int hex_to_bytes(const char *hex_str, uint8_t *bytes, size_t len);

#endif // CRYPTO_UTILS_H
