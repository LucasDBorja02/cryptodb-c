#include "util/secure_mem.h"
#include <string.h>

void secure_bzero(void* p, size_t n) {
#if defined(__STDC_LIB_EXT1__)
    memset_s(p, n, 0, n);
#elif defined(_WIN32)
    SecureZeroMemory(p, n);
#else
    volatile unsigned char* vp = (volatile unsigned char*)p;
    while (n--) { *vp++ = 0; }
#endif
}
