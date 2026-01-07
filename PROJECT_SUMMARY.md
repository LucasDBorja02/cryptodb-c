# Project Summary - cryptodb-c

## Implementation Overview

This project successfully implements a complete applied cryptography system in C with PostgreSQL database integration, demonstrating encryption at rest for sensitive data storage.

## What Was Built

### 1. Authorial Symmetric Cipher (src/cipher.c - 11KB)
- Custom block cipher based on Substitution-Permutation Network (SPN) architecture
- Key specifications:
  - Block size: 128 bits (16 bytes)
  - Key size: 256 bits (32 bytes)
  - Rounds: 10
- Components:
  - S-box substitution layer (256-byte lookup table)
  - Permutation layer for byte reordering
  - MixColumns for linear diffusion
  - Round key generation with key schedule
  - Inverse operations for decryption

### 2. CBC Mode Implementation (src/cipher.c)
- Cipher Block Chaining mode with:
  - Cryptographically secure random IV generation
  - PKCS7 padding (handles all input sizes correctly)
  - Proper block chaining for security
  - Memory-safe allocation and cleanup

### 3. Cryptographic Utilities (src/crypto_utils.c - 2KB)
- Random number generation using /dev/urandom
- PKCS7 padding add/remove with validation
- Secure hex encoding/decoding with bounds checking
- Uses snprintf for buffer safety

### 4. PostgreSQL Integration (src/database.c - 4.7KB)
- Database connection management
- Encrypted data storage (ciphertext + IV)
- Record retrieval and listing
- Warning on overwrites
- SQL injection protection via parameterized queries

### 5. Command-Line Application (src/main.c - 6.4KB)
- Encrypt command with random key/IV generation
- Decrypt command with key validation
- List command for viewing stored records
- Input validation (hex format, length checks)
- Safe input handling with fgets

### 6. Database Schema (sql/schema.sql)
- Table: encrypted_data
  - id (SERIAL PRIMARY KEY)
  - name (VARCHAR UNIQUE)
  - ciphertext (TEXT - hex encoded)
  - iv (VARCHAR - hex encoded)
  - created_at (TIMESTAMP)
- Indexes for performance

### 7. Build System (Makefile)
- Clean compilation with -Wall -Wextra -Werror
- Dependency management
- Database setup target
- Clean target

### 8. Documentation
- README.md: Comprehensive user guide with examples
- TECHNICAL.md: Algorithm details and API reference
- QUICKSTART.md: Fast-start guide for new users
- CONTRIBUTING.md: Contribution guidelines
- demo.sh: Demonstration script
- .env.example: Configuration template

## Security Features Implemented

1. **Cryptographically Secure Random Generation**: Uses /dev/urandom for IVs and keys
2. **CBC Mode**: Prevents pattern recognition in encrypted data
3. **Unique IVs**: Each encryption uses a fresh random IV
4. **Strong Keys**: 256-bit keys resist brute force attacks
5. **Input Validation**: Hex input validation, length checks
6. **Safe String Handling**: Uses fgets, snprintf instead of unsafe functions
7. **Memory Safety**: Proper allocation, bounds checking, cleanup
8. **SQL Injection Protection**: Parameterized queries
9. **Padding Validation**: PKCS7 padding verification on decryption

## Code Statistics

- Total source code: ~802 lines
- Source files: 4 (.c files)
- Header files: 3 (.h files)
- SQL schema: 1 file
- Documentation: 5 markdown files
- Build system: 1 Makefile

## Security Improvements Made

1. Fixed cipher decryption inverse permutation
2. Corrected PKCS7 padding for block-aligned data
3. Added hex input validation for keys
4. Added warning on database record overwrites
5. Replaced scanf with fgets for safer input
6. Replaced sprintf with snprintf for buffer safety
7. Added validation to hex_to_bytes conversion

## Usage Example

```bash
# Build
make

# Setup database
make setup-db

# Encrypt data
./cryptodb encrypt secret "My confidential message"
# Output: Key (save this): a1b2c3d4e5f6...

# List records
./cryptodb list

# Decrypt data
./cryptodb decrypt secret
# Enter key when prompted
# Output: Decrypted data: My confidential message
```

## Technologies Used

- **Language**: C
- **Database**: PostgreSQL with libpq
- **Compiler**: GCC with strict warnings
- **Build System**: Make
- **Platform**: Linux/Unix

## Educational Value

This project demonstrates:
1. How to design a custom block cipher
2. Proper implementation of CBC mode
3. Secure random number generation
4. Database integration for encrypted storage
5. Key management considerations
6. Input validation and sanitization
7. Memory-safe C programming
8. Practical cryptography in real-world scenarios

## Limitations and Disclaimers

1. **Educational Purpose**: Not audited by cryptography experts
2. **Key Display**: Keys shown in plaintext for demonstration
3. **Custom Cipher**: Not battle-tested like AES or ChaCha20
4. **No Authentication**: Lacks message authentication (HMAC)
5. **Side Channels**: Not hardened against timing attacks

## Production Recommendations

For production use, consider:
1. Using established ciphers (AES-256-GCM)
2. Implementing proper key management (KMS, HSM)
3. Adding message authentication codes
4. Using authenticated encryption modes
5. Implementing secure key derivation (PBKDF2, Argon2)
6. Hardening against side-channel attacks
7. Regular security audits

## Project Structure

```
cryptodb-c/
├── include/           # Header files
│   ├── cipher.h       # Cipher declarations
│   ├── crypto_utils.h # Utility declarations
│   └── database.h     # Database declarations
├── src/               # Implementation
│   ├── cipher.c       # Cipher implementation
│   ├── crypto_utils.c # Utilities implementation
│   ├── database.c     # Database implementation
│   └── main.c         # Main application
├── sql/               # Database
│   └── schema.sql     # Table definitions
├── Makefile           # Build system
├── README.md          # User documentation
├── TECHNICAL.md       # Technical details
├── QUICKSTART.md      # Quick start guide
├── CONTRIBUTING.md    # Contribution guide
├── demo.sh            # Demo script
└── .env.example       # Config template
```

## Key Achievements

✓ Complete symmetric cipher implementation from scratch
✓ Proper CBC mode with random IVs
✓ Database integration with PostgreSQL
✓ Secure coding practices (input validation, safe functions)
✓ Comprehensive documentation
✓ Working command-line interface
✓ Build system and setup automation
✓ Security improvements based on code review

## Conclusion

This project successfully implements all requirements from the problem statement:
- ✅ Applied cryptography in C
- ✅ PostgreSQL integration
- ✅ Authorial symmetric cipher
- ✅ CBC mode operation
- ✅ Random IVs
- ✅ Encryption at rest
- ✅ Secure storage demonstration
- ✅ Practical database integration

The implementation is complete, well-documented, and demonstrates practical cryptography with secure coding practices suitable for educational purposes.
