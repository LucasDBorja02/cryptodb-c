#include "util/hex.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    c = (char)tolower((unsigned char)c);
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    return -1;
}

int hex_encode(const uint8_t* in, size_t in_len, char** out_str) {
    if (!in || !out_str) return -1;
    static const char* hexd = "0123456789abcdef";
    size_t out_len = in_len * 2;
    char* s = (char*)malloc(out_len + 1);
    if (!s) return -2;
    for (size_t i = 0; i < in_len; i++) {
        s[i*2]   = hexd[(in[i] >> 4) & 0xF];
        s[i*2+1] = hexd[in[i] & 0xF];
    }
    s[out_len] = '\0';
    *out_str = s;
    return 0;
}

int hex_decode(const char* hex, uint8_t** out, size_t* out_len) {
    if (!hex || !out || !out_len) return -1;
    size_t n = strlen(hex);
    if (n % 2 != 0) return -2;
    uint8_t* buf = (uint8_t*)malloc(n/2);
    if (!buf) return -3;
    for (size_t i = 0; i < n; i += 2) {
        int a = hexval(hex[i]);
        int b = hexval(hex[i+1]);
        if (a < 0 || b < 0) { free(buf); return -4; }
        buf[i/2] = (uint8_t)((a << 4) | b);
    }
    *out = buf;
    *out_len = n/2;
    return 0;
}
