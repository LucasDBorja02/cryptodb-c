#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void secure_bzero(void* p, size_t n);

#ifdef __cplusplus
}
#endif
