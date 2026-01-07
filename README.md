# cryptodb-c

## Overview / Visão Geral

**EN:** Applied cryptography project in C with PostgreSQL. Implements an authorial symmetric cipher using CBC mode and random IVs for encryption at rest, demonstrating secure storage of sensitive data and practical database integration.

**PT-BR:** Projeto de criptografia aplicada em C com PostgreSQL. Implementa um algoritmo simétrico autoral com modo CBC e IVs aleatórios para criptografia de dados em repouso, com armazenamento seguro e integração com banco de dados.

## Features / Características

- **Custom Symmetric Cipher**: Authorial block cipher based on Substitution-Permutation Network (SPN)
- **CBC Mode**: Cipher Block Chaining mode for encryption
- **Random IVs**: Cryptographically secure random Initialization Vectors
- **PostgreSQL Integration**: Secure storage of encrypted data in database
- **PKCS7 Padding**: Standard padding for block cipher operations
- **256-bit Keys**: Strong key size for encryption
- **128-bit Blocks**: Standard block size for security and performance

## Architecture / Arquitetura

```
cryptodb-c/
├── include/           # Header files
│   ├── cipher.h       # Cipher and CBC mode declarations
│   ├── crypto_utils.h # Cryptographic utilities
│   └── database.h     # Database operations
├── src/               # Source files
│   ├── cipher.c       # Authorial cipher implementation
│   ├── crypto_utils.c # Random generation, padding, hex conversion
│   ├── database.c     # PostgreSQL integration
│   └── main.c         # Main application
├── sql/               # SQL schemas
│   └── schema.sql     # Database schema
└── Makefile           # Build configuration
```

## Cipher Design / Design do Algoritmo

The authorial symmetric cipher is based on the following principles:

1. **Substitution Layer**: Uses custom S-box for byte substitution
2. **Permutation Layer**: Permutes bytes within the block
3. **Mixing Layer**: Linear transformation for diffusion
4. **Key Schedule**: Round key generation from master key
5. **10 Rounds**: Multiple rounds for security

### CBC Mode Implementation

- Each block is XORed with the previous ciphertext block before encryption
- First block uses the random IV
- Provides security even when encrypting identical plaintext blocks

## Requirements / Requisitos

- GCC compiler
- PostgreSQL 12 or higher
- libpq-dev (PostgreSQL client library)
- Linux/Unix environment

## Installation / Instalação

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y postgresql postgresql-contrib libpq-dev gcc make
```

### 2. Setup PostgreSQL

Start PostgreSQL service:
```bash
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

Create database and schema:
```bash
# Create database
sudo -u postgres psql -c "CREATE DATABASE cryptodb;"

# Create schema
sudo -u postgres psql cryptodb -f sql/schema.sql
```

Or use the Makefile:
```bash
make setup-db
```

### 3. Build the Project

```bash
make
```

This will compile the project and create the `cryptodb` executable.

## Usage / Uso

### Environment Variables

Set the PostgreSQL connection string (optional):
```bash
export DB_CONNINFO="host=localhost dbname=cryptodb user=postgres password=postgres"
```

### Encrypt Data

```bash
./cryptodb encrypt <name> "<plaintext>"
```

Example:
```bash
./cryptodb encrypt secret "My confidential message"
```

Output:
```
Data encrypted and stored successfully!
Name: secret
Key (save this for decryption): a1b2c3d4e5f6...
```

**⚠️ IMPORTANT**: Save the encryption key! You'll need it to decrypt the data.

### Decrypt Data

```bash
./cryptodb decrypt <name>
```

Example:
```bash
./cryptodb decrypt secret
# Enter the encryption key when prompted
```

### List Stored Records

```bash
./cryptodb list
```

## Security Features / Recursos de Segurança

1. **Random IV Generation**: Uses `/dev/urandom` for cryptographically secure random numbers
2. **CBC Mode**: Prevents pattern recognition in encrypted data
3. **PKCS7 Padding**: Standard padding scheme with validation
4. **Unique IVs**: Each encryption uses a new random IV
5. **Strong Keys**: 256-bit keys provide high security
6. **Database Storage**: Encrypted data and IVs stored separately from keys

## Security Considerations / Considerações de Segurança

**EN:**
- This is an educational/demonstration project
- Keys are displayed in plaintext for demonstration purposes
- In production, use proper key management (HSM, KMS, etc.)
- The authorial cipher has not been audited by cryptography experts
- For production use, consider established algorithms (AES, ChaCha20)

**PT-BR:**
- Este é um projeto educacional/demonstrativo
- As chaves são exibidas em texto claro para fins de demonstração
- Em produção, use gerenciamento adequado de chaves (HSM, KMS, etc.)
- O algoritmo autoral não foi auditado por especialistas em criptografia
- Para uso em produção, considere algoritmos estabelecidos (AES, ChaCha20)

## Example Workflow / Fluxo de Exemplo

```bash
# 1. Encrypt sensitive data
./cryptodb encrypt "user_password" "MySecretP@ssw0rd"
# Save the key: 4f3e2d1c...

# 2. List all stored records
./cryptodb list
# Shows: user_password    2024-01-07 22:00:00

# 3. Decrypt the data
./cryptodb decrypt "user_password"
# Enter key: 4f3e2d1c...
# Output: Decrypted data: MySecretP@ssw0rd
```

## Database Schema / Esquema do Banco

```sql
CREATE TABLE encrypted_data (
    id SERIAL PRIMARY KEY,
    name VARCHAR(255) UNIQUE NOT NULL,
    ciphertext TEXT NOT NULL,
    iv VARCHAR(32) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

## Building and Testing / Compilação e Testes

### Build
```bash
make
```

### Clean
```bash
make clean
```

### Run
```bash
make run
```

## Technical Details / Detalhes Técnicos

### Cipher Specifications
- **Block Size**: 128 bits (16 bytes)
- **Key Size**: 256 bits (32 bytes)
- **IV Size**: 128 bits (16 bytes)
- **Mode**: CBC (Cipher Block Chaining)
- **Padding**: PKCS7
- **Rounds**: 10

### Cryptographic Operations
1. Key schedule generates 10 round keys
2. Each round performs: SubBytes → PermBytes → MixColumns → AddRoundKey
3. Final round omits MixColumns
4. Decryption reverses all operations

## License

MIT License - See LICENSE file for details

## Contributing / Contribuindo

Contributions are welcome! Please feel free to submit issues or pull requests.

## Author / Autor

Project for applied cryptography studies with C and PostgreSQL.
