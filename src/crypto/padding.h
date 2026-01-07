#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// PKCS#7 padding for block_size (must be 1..255)
int pkcs7_pad(const uint8_t* in, size_t in_len, size_t block_size, uint8_t** out, size_t* out_len);
int pkcs7_unpad(uint8_t* inout, size_t inout_len, size_t block_size, size_t* out_len);

#ifdef __cplusplus
}
#endif
