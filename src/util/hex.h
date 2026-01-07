#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int hex_encode(const uint8_t* in, size_t in_len, char** out_str); // mallocs
int hex_decode(const char* hex, uint8_t** out, size_t* out_len);  // mallocs

#ifdef __cplusplus
}
#endif
