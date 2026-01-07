#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crypto/xorfeistel.h"
#include "crypto/cbc.h"
#include "crypto/aes_openssl.h"
#include "util/secure_mem.h"

static int arg_eq(const char* a, const char* b) { return a && b && strcmp(a,b)==0; }

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void usage(void) {
    puts("cryptodb_bench --mb <N> --key <pass>\n"
         "  --mb  data size in MB (default 64)\n");
}

int main(int argc, char** argv) {
    size_t mb = 64;
    const char* key = "benchmark-key";

    for (int i = 1; i < argc; i++) {
        if (arg_eq(argv[i], "--mb") && i+1 < argc) mb = (size_t)atoi(argv[++i]);
        else if (arg_eq(argv[i], "--key") && i+1 < argc) key = argv[++i];
    }

    size_t bytes = mb * 1024ULL * 1024ULL;
    uint8_t* data = (uint8_t*)malloc(bytes);
    if (!data) { fprintf(stderr, "alloc failed\n"); return 1; }
    // simple deterministic fill (benchmark)
    for (size_t i = 0; i < bytes; i++) data[i] = (uint8_t)(i * 1315423911u);

    // --------- XFS (autoral) ----------
    xfs_ctx_t ctx;
    if (xfs_init(&ctx, (const uint8_t*)key, strlen(key), 16) != 0) {
        fprintf(stderr, "xfs_init failed\n");
        free(data);
        return 2;
    }

    double t0 = now_sec();
    uint8_t* xfs_ct = NULL; size_t xfs_ct_len = 0;
    if (xfs_cbc_encrypt(&ctx, data, bytes, NULL, &xfs_ct, &xfs_ct_len) != 0) {
        fprintf(stderr, "xfs encrypt failed\n");
        free(data);
        return 3;
    }
    double t1 = now_sec();

    uint8_t* xfs_pt = NULL; size_t xfs_pt_len = 0;
    if (xfs_cbc_decrypt(&ctx, xfs_ct, xfs_ct_len, &xfs_pt, &xfs_pt_len) != 0) {
        fprintf(stderr, "xfs decrypt failed\n");
        free(data); free(xfs_ct);
        return 4;
    }
    double t2 = now_sec();

    // --------- AES-256-CBC (OpenSSL) ----------
    double t3 = now_sec();
    uint8_t* aes_ct = NULL; size_t aes_ct_len = 0;
    if (aes256_cbc_encrypt_passphrase(data, bytes, key, &aes_ct, &aes_ct_len) != 0) {
        fprintf(stderr, "aes encrypt failed\n");
        free(data); free(xfs_ct); free(xfs_pt);
        return 5;
    }
    double t4 = now_sec();

    uint8_t* aes_pt = NULL; size_t aes_pt_len = 0;
    if (aes256_cbc_decrypt_passphrase(aes_ct, aes_ct_len, key, &aes_pt, &aes_pt_len) != 0) {
        fprintf(stderr, "aes decrypt failed\n");
        free(data); free(xfs_ct); free(xfs_pt); free(aes_ct);
        return 6;
    }
    double t5 = now_sec();

    // Validate sizes (padding means plaintext may be exact)
    if (xfs_pt_len != bytes || aes_pt_len != bytes || memcmp(data, xfs_pt, bytes) != 0 || memcmp(data, aes_pt, bytes) != 0) {
        fprintf(stderr, "validation failed\n");
    }

    double xfs_enc = t1 - t0;
    double xfs_dec = t2 - t1;
    double aes_enc = t4 - t3;
    double aes_dec = t5 - t4;

    printf("Data: %zu MB\n", mb);
    printf("XFS (autoral)  encrypt: %.3fs (%.1f MB/s)\n", xfs_enc, (double)mb / xfs_enc);
    printf("XFS (autoral)  decrypt: %.3fs (%.1f MB/s)\n", xfs_dec, (double)mb / xfs_dec);
    printf("AES-256-CBC    encrypt: %.3fs (%.1f MB/s)\n", aes_enc, (double)mb / aes_enc);
    printf("AES-256-CBC    decrypt: %.3fs (%.1f MB/s)\n", aes_dec, (double)mb / aes_dec);

    secure_bzero(&ctx, sizeof(ctx));
    secure_bzero(xfs_ct, xfs_ct_len);
    secure_bzero(xfs_pt, xfs_pt_len);
    secure_bzero(aes_ct, aes_ct_len);
    secure_bzero(aes_pt, aes_pt_len);

    free(data);
    free(xfs_ct); free(xfs_pt);
    free(aes_ct); free(aes_pt);
    return 0;
}
