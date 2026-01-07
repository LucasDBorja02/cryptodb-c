#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XFS_BLOCK_SIZE 16u   // 128-bit block
#define XFS_KEY_MAX    64u   // up to 64 bytes (educational)

typedef struct {
    uint32_t rounds;
    uint8_t  round_keys[32][16]; // up to 32 rounds; 16-byte subkey each
} xfs_ctx_t;

// Initializes context from user key (bytes) and number of rounds (recommended: 16)
int xfs_init(xfs_ctx_t* ctx, const uint8_t* key, size_t key_len, uint32_t rounds);

// Encrypt/decrypt exactly one 16-byte block (ECB-style primitive)
void xfs_encrypt_block(const xfs_ctx_t* ctx, const uint8_t in[XFS_BLOCK_SIZE], uint8_t out[XFS_BLOCK_SIZE]);
void xfs_decrypt_block(const xfs_ctx_t* ctx, const uint8_t in[XFS_BLOCK_SIZE], uint8_t out[XFS_BLOCK_SIZE]);

#ifdef __cplusplus
}
#endif
