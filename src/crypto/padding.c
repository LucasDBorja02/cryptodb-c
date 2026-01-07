#include "crypto/padding.h"
#include <stdlib.h>
#include <string.h>

int pkcs7_pad(const uint8_t* in, size_t in_len, size_t block_size, uint8_t** out, size_t* out_len) {
    if (!out || !out_len || block_size == 0 || block_size > 255) return -1;
    size_t pad = block_size - (in_len % block_size);
    if (pad == 0) pad = block_size;
    size_t total = in_len + pad;
    uint8_t* buf = (uint8_t*)malloc(total);
    if (!buf) return -2;
    if (in_len && in) memcpy(buf, in, in_len);
    memset(buf + in_len, (int)pad, pad);
    *out = buf;
    *out_len = total;
    return 0;
}

int pkcs7_unpad(uint8_t* inout, size_t inout_len, size_t block_size, size_t* out_len) {
    if (!inout || !out_len || block_size == 0 || block_size > 255) return -1;
    if (inout_len == 0 || (inout_len % block_size) != 0) return -2;
    uint8_t pad = inout[inout_len - 1];
    if (pad == 0 || pad > block_size) return -3;
    // Verify padding bytes (constant-ish time)
    size_t start = inout_len - pad;
    unsigned bad = 0;
    for (size_t i = start; i < inout_len; i++) bad |= (inout[i] != pad);
    if (bad) return -4;
    *out_len = inout_len - pad;
    return 0;
}
