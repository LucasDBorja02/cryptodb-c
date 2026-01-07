# Technical Documentation - cryptodb-c

## Cipher Algorithm Design

### Overview
The authorial symmetric cipher implements a Substitution-Permutation Network (SPN) structure with the following components:

### Components

#### 1. S-Box (Substitution Box)
- 256-byte lookup table for byte substitution
- Provides confusion in the cipher
- Inverse S-box used for decryption
- Based on nonlinear transformation

#### 2. Permutation Table
- 16-byte permutation for byte reordering
- Provides diffusion within blocks
- Fixed permutation pattern: `[0, 13, 10, 7, 4, 1, 14, 11, 8, 5, 2, 15, 12, 9, 6, 3]`

#### 3. MixColumns Operation
- Linear transformation for diffusion
- Operates on 4-byte columns
- Formula for each column:
  ```
  a' = a ⊕ b ⊕ c
  b' = b ⊕ c ⊕ d
  c' = c ⊕ d ⊕ a
  d' = d ⊕ a ⊕ b
  ```
- Inverse operation for decryption

#### 4. Key Schedule
- Generates 10 round keys from master key
- Each round key is 128 bits (16 bytes)
- Incorporates round constants and S-box transformations
- Formula: `round_key[i] = key[i % 32] ⊕ (round * 17 + i * 3) ⊕ SBOX[(round + i) % 256]`

### Encryption Process

```
Input: 128-bit plaintext block, 256-bit key
Output: 128-bit ciphertext block

1. Initial round key addition
   state ← state ⊕ round_key[0]

2. Main rounds (9 iterations):
   For round = 1 to 9:
     a. SubBytes(state) - Apply S-box
     b. PermBytes(state) - Permute bytes
     c. MixColumns(state) - Linear mixing
     d. AddRoundKey(state, round_key[round])

3. Final round (no MixColumns):
   a. SubBytes(state)
   b. PermBytes(state)

4. Return state as ciphertext
```

### Decryption Process

```
Input: 128-bit ciphertext block, 256-bit key
Output: 128-bit plaintext block

1. Reverse final round:
   a. InvPermBytes(state)
   b. InvSubBytes(state) - Apply inverse S-box

2. Main rounds in reverse (9 iterations):
   For round = 9 to 1:
     a. AddRoundKey(state, round_key[round])
     b. InvMixColumns(state)
     c. InvPermBytes(state)
     d. InvSubBytes(state)

3. Remove initial round key:
   state ← state ⊕ round_key[0]

4. Return state as plaintext
```

## CBC Mode Implementation

### Encryption

```
IV ← random 128-bit value
prev ← IV

For each plaintext block P[i]:
  temp ← P[i] ⊕ prev
  C[i] ← Encrypt(temp, key)
  prev ← C[i]

Return (C, IV)
```

### Decryption

```
prev ← IV

For each ciphertext block C[i]:
  temp ← Decrypt(C[i], key)
  P[i] ← temp ⊕ prev
  prev ← C[i]

Return P
```

## PKCS7 Padding

### Adding Padding
- Calculate padding length: `pad_len = block_size - (data_len % block_size)`
- Append `pad_len` bytes, each with value `pad_len`
- Example: For block_size=16, data_len=13 → append 3 bytes of value 0x03

### Removing Padding
- Read last byte to get padding length
- Validate all padding bytes have correct value
- Remove padding bytes to recover original data

## Security Considerations

### Strengths
1. **CBC Mode**: Prevents pattern recognition in ciphertext
2. **Random IVs**: Each encryption produces different ciphertext
3. **Multiple Rounds**: 10 rounds provide security margin
4. **Strong Key Size**: 256-bit keys resist brute force
5. **Proper Padding**: PKCS7 with validation

### Limitations
1. **Not Cryptanalyzed**: The authorial cipher hasn't been reviewed by experts
2. **Educational Purpose**: Designed for learning, not production
3. **Key Management**: Keys displayed in plaintext for demonstration
4. **No Authentication**: No built-in message authentication (consider adding HMAC)
5. **Side-Channel Attacks**: Implementation not hardened against timing attacks

### Recommendations for Production
1. Use established ciphers (AES-256)
2. Implement proper key management (KMS, HSM)
3. Add message authentication (HMAC-SHA256)
4. Use authenticated encryption modes (GCM, CCM)
5. Implement secure key derivation (PBKDF2, Argon2)
6. Protect against timing attacks
7. Regular security audits

## Database Schema

### Table: encrypted_data

```sql
Column      | Type         | Description
------------|--------------|------------------------------------------
id          | SERIAL       | Auto-incrementing primary key
name        | VARCHAR(255) | Unique identifier for encrypted data
ciphertext  | TEXT         | Encrypted data in hexadecimal format
iv          | VARCHAR(32)  | Initialization vector (16 bytes as hex)
created_at  | TIMESTAMP    | Record creation timestamp
```

### Indexes
- Primary key on `id`
- Unique constraint on `name`
- Index on `name` for faster lookups

## API Reference

### cipher.h

#### `void cipher_encrypt_block(const uint8_t *input, uint8_t *output, const uint8_t *key)`
Encrypts a single 128-bit block.
- **input**: 16-byte plaintext block
- **output**: 16-byte ciphertext block
- **key**: 32-byte encryption key

#### `void cipher_decrypt_block(const uint8_t *input, uint8_t *output, const uint8_t *key)`
Decrypts a single 128-bit block.
- **input**: 16-byte ciphertext block
- **output**: 16-byte plaintext block
- **key**: 32-byte decryption key

#### `int cbc_encrypt(const uint8_t *plaintext, size_t plaintext_len, const uint8_t *key, const uint8_t *iv, uint8_t **ciphertext, size_t *ciphertext_len)`
Encrypts data using CBC mode.
- Returns 0 on success, -1 on failure
- Allocates memory for ciphertext (caller must free)

#### `int cbc_decrypt(const uint8_t *ciphertext, size_t ciphertext_len, const uint8_t *key, const uint8_t *iv, uint8_t **plaintext, size_t *plaintext_len)`
Decrypts data using CBC mode.
- Returns 0 on success, -1 on failure
- Allocates memory for plaintext (caller must free)

### crypto_utils.h

#### `int generate_random_bytes(uint8_t *buffer, size_t length)`
Generates cryptographically secure random bytes.
- Uses `/dev/urandom`
- Returns 0 on success, -1 on failure

#### `size_t add_pkcs7_padding(uint8_t *data, size_t data_len, size_t block_size)`
Adds PKCS7 padding to data.
- Returns total length after padding

#### `int remove_pkcs7_padding(uint8_t *data, size_t data_len, size_t *unpadded_len)`
Removes and validates PKCS7 padding.
- Returns 0 on success, -1 on invalid padding

#### `void bytes_to_hex(const uint8_t *bytes, size_t len, char *hex_str)`
Converts binary data to hexadecimal string.

#### `void hex_to_bytes(const char *hex_str, uint8_t *bytes, size_t len)`
Converts hexadecimal string to binary data.

### database.h

#### `PGconn* db_connect(const char *conninfo)`
Connects to PostgreSQL database.
- Returns connection handle or NULL on failure

#### `void db_disconnect(PGconn *conn)`
Closes database connection.

#### `int db_store_encrypted_data(PGconn *conn, const char *name, const uint8_t *ciphertext, size_t ciphertext_len, const uint8_t *iv)`
Stores encrypted data in database.
- Returns 0 on success, -1 on failure

#### `int db_retrieve_encrypted_data(PGconn *conn, const char *name, uint8_t **ciphertext, size_t *ciphertext_len, uint8_t **iv)`
Retrieves encrypted data from database.
- Returns 0 on success, -1 on failure
- Allocates memory for ciphertext and IV (caller must free)

#### `int db_list_records(PGconn *conn)`
Lists all stored records.
- Returns 0 on success, -1 on failure

## Performance Considerations

### Time Complexity
- Single block encryption/decryption: O(1)
- Full message encryption/decryption: O(n) where n is number of blocks
- Database operations: O(1) for single record, O(log n) for list

### Memory Usage
- Key schedule: 160 bytes (10 round keys × 16 bytes)
- CBC encryption: O(n) additional memory for padded data
- Database: Ciphertext stored as hex (2x original size)

### Optimization Opportunities
1. Precompute round keys for repeated operations
2. Use table lookups for faster S-box operations
3. Implement SIMD instructions for parallel operations
4. Cache database connections

## Testing Guide

### Manual Testing

#### Test 1: Basic Encryption/Decryption
```bash
./cryptodb encrypt test1 "Hello World"
# Save the key
./cryptodb decrypt test1
# Enter the saved key
# Verify output matches input
```

#### Test 2: Long Messages
```bash
./cryptodb encrypt test2 "This is a much longer message that spans multiple blocks and tests the CBC mode implementation with padding"
./cryptodb decrypt test2
```

#### Test 3: Special Characters
```bash
./cryptodb encrypt test3 "Special: !@#$%^&*()_+-=[]{}|;:',.<>?/~"
./cryptodb decrypt test3
```

#### Test 4: Empty String
```bash
./cryptodb encrypt test4 ""
./cryptodb decrypt test4
```

#### Test 5: Binary Data (if applicable)
Test with various input sizes to ensure padding works correctly.

### Validation Checks
1. Decrypted data matches original plaintext
2. Different IVs produce different ciphertexts for same plaintext
3. Wrong key fails decryption
4. Database correctly stores and retrieves data
5. List command shows all records

## Troubleshooting

### Common Issues

#### Connection Failed
- Ensure PostgreSQL is running: `sudo systemctl status postgresql`
- Check connection string in DB_CONNINFO
- Verify database exists: `sudo -u postgres psql -l`

#### Compilation Errors
- Install libpq-dev: `sudo apt-get install libpq-dev`
- Check GCC version: `gcc --version`
- Verify PostgreSQL headers: `ls /usr/include/postgresql`

#### Decryption Failed
- Verify you're using the correct key
- Check that the data exists: `./cryptodb list`
- Ensure database hasn't been corrupted

#### Memory Issues
- Check available memory: `free -h`
- Verify no memory leaks with valgrind: `valgrind ./cryptodb encrypt test "data"`
