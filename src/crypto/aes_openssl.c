#include "crypto/aes_openssl.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>

static void sha256_key_from_pass(const char* pass, uint8_t out32[32]) {
    SHA256_CTX c;
    SHA256_Init(&c);
    SHA256_Update(&c, (const unsigned char*)pass, strlen(pass));
    SHA256_Final(out32, &c);
}

int aes256_cbc_encrypt_passphrase(const uint8_t* plaintext, size_t pt_len,
                                  const char* passphrase,
                                  uint8_t** out, size_t* out_len) {
    if (!plaintext || !passphrase || !out || !out_len) return -1;

    uint8_t key[32];
    uint8_t iv[16];
    sha256_key_from_pass(passphrase, key);
    if (RAND_bytes(iv, sizeof(iv)) != 1) return -2;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -3;

    int rc = -4;
    int len = 0, cipher_len = 0;
    // ciphertext size: pt_len + block
    uint8_t* buf = (uint8_t*)malloc(16 + pt_len + 16);
    if (!buf) { EVP_CIPHER_CTX_free(ctx); return -5; }
    memcpy(buf, iv, 16);

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) goto cleanup;
    if (EVP_EncryptUpdate(ctx, buf + 16, &len, plaintext, (int)pt_len) != 1) goto cleanup;
    cipher_len = len;
    if (EVP_EncryptFinal_ex(ctx, buf + 16 + cipher_len, &len) != 1) goto cleanup;
    cipher_len += len;

    *out = buf;
    *out_len = 16 + (size_t)cipher_len;
    rc = 0;

cleanup:
    if (rc != 0) free(buf);
    EVP_CIPHER_CTX_free(ctx);
    return rc;
}

int aes256_cbc_decrypt_passphrase(const uint8_t* in, size_t in_len,
                                  const char* passphrase,
                                  uint8_t** plaintext, size_t* pt_len) {
    if (!in || !passphrase || !plaintext || !pt_len) return -1;
    if (in_len < 16) return -2;

    const uint8_t* iv = in;
    const uint8_t* ct = in + 16;
    size_t ct_len = in_len - 16;

    uint8_t key[32];
    sha256_key_from_pass(passphrase, key);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -3;

    int rc = -4;
    int len = 0, outl = 0;

    uint8_t* buf = (uint8_t*)malloc(ct_len + 16 + 1);
    if (!buf) { EVP_CIPHER_CTX_free(ctx); return -5; }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) goto cleanup;
    if (EVP_DecryptUpdate(ctx, buf, &len, ct, (int)ct_len) != 1) goto cleanup;
    outl = len;
    if (EVP_DecryptFinal_ex(ctx, buf + outl, &len) != 1) goto cleanup;
    outl += len;
    buf[outl] = 0;

    *plaintext = buf;
    *pt_len = (size_t)outl;
    rc = 0;

cleanup:
    if (rc != 0) free(buf);
    EVP_CIPHER_CTX_free(ctx);
    return rc;
}
