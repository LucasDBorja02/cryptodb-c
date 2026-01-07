#include "crypto/cbc.h"
#include "crypto/padding.h"
#include <openssl/rand.h>
#include <stdlib.h>
#include <string.h>

int xfs_cbc_encrypt(const xfs_ctx_t* ctx,
                    const uint8_t* plaintext, size_t pt_len,
                    const uint8_t iv[XFS_BLOCK_SIZE],
                    uint8_t** out, size_t* out_len) {
    if (!ctx || !out || !out_len) return -1;

    uint8_t* padded = NULL;
    size_t padded_len = 0;
    int rc = pkcs7_pad(plaintext, pt_len, XFS_BLOCK_SIZE, &padded, &padded_len);
    if (rc != 0) return -2;

    size_t total = XFS_BLOCK_SIZE + padded_len; // IV + ciphertext
    uint8_t* buf = (uint8_t*)malloc(total);
    if (!buf) { free(padded); return -3; }

    uint8_t local_iv[XFS_BLOCK_SIZE];
    if (iv) memcpy(local_iv, iv, XFS_BLOCK_SIZE);
    else {
        if (RAND_bytes(local_iv, XFS_BLOCK_SIZE) != 1) { free(padded); free(buf); return -4; }
    }
    memcpy(buf, local_iv, XFS_BLOCK_SIZE);

    uint8_t prev[XFS_BLOCK_SIZE];
    memcpy(prev, local_iv, XFS_BLOCK_SIZE);

    for (size_t off = 0; off < padded_len; off += XFS_BLOCK_SIZE) {
        uint8_t block[XFS_BLOCK_SIZE];
        memcpy(block, padded + off, XFS_BLOCK_SIZE);
        for (int i = 0; i < (int)XFS_BLOCK_SIZE; i++) block[i] ^= prev[i]; // CBC XOR

        uint8_t enc[XFS_BLOCK_SIZE];
        xfs_encrypt_block(ctx, block, enc);
        memcpy(buf + XFS_BLOCK_SIZE + off, enc, XFS_BLOCK_SIZE);
        memcpy(prev, enc, XFS_BLOCK_SIZE);
    }

    free(padded);
    *out = buf;
    *out_len = total;
    return 0;
}

int xfs_cbc_decrypt(const xfs_ctx_t* ctx,
                    const uint8_t* in, size_t in_len,
                    uint8_t** plaintext, size_t* pt_len) {
    if (!ctx || !in || !plaintext || !pt_len) return -1;
    if (in_len < XFS_BLOCK_SIZE || ((in_len - XFS_BLOCK_SIZE) % XFS_BLOCK_SIZE) != 0) return -2;

    const uint8_t* iv = in;
    const uint8_t* ct = in + XFS_BLOCK_SIZE;
    size_t ct_len = in_len - XFS_BLOCK_SIZE;

    uint8_t* buf = (uint8_t*)malloc(ct_len);
    if (!buf) return -3;

    uint8_t prev[XFS_BLOCK_SIZE];
    memcpy(prev, iv, XFS_BLOCK_SIZE);

    for (size_t off = 0; off < ct_len; off += XFS_BLOCK_SIZE) {
        uint8_t dec[XFS_BLOCK_SIZE];
        xfs_decrypt_block(ctx, ct + off, dec);
        for (int i = 0; i < (int)XFS_BLOCK_SIZE; i++) dec[i] ^= prev[i];
        memcpy(buf + off, dec, XFS_BLOCK_SIZE);
        memcpy(prev, ct + off, XFS_BLOCK_SIZE);
    }

    size_t unpadded_len = 0;
    int rc = pkcs7_unpad(buf, ct_len, XFS_BLOCK_SIZE, &unpadded_len);
    if (rc != 0) { free(buf); return -4; }

    // shrink to exact plaintext size
    uint8_t* out = (uint8_t*)malloc(unpadded_len + 1);
    if (!out) { free(buf); return -5; }
    memcpy(out, buf, unpadded_len);
    out[unpadded_len] = 0; // convenient null terminator for text fields
    free(buf);

    *plaintext = out;
    *pt_len = unpadded_len;
    return 0;
}
