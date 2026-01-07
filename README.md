# CryptoDB — Cryptography in C with PostgreSQL

Projeto acadêmico/técnico de **criptografia aplicada**, implementado em **C**, integrando
um algoritmo criptográfico simétrico autoral com **PostgreSQL**, focado em
**criptografia de dados em repouso**.

## 🔐 Features

- Algoritmo criptográfico simétrico autoral (estrutura tipo Feistel)
- Modo de operação CBC com IV aleatório
- Criptografia de campos sensíveis (CPF, email)
- Armazenamento seguro em PostgreSQL (`BYTEA`)
- Descriptografia apenas na aplicação
- Testes de não-determinismo e efeito avalanche
- Benchmark comparativo com AES-256-CBC (OpenSSL)

## 🧠 Conceitos aplicados

- XOR, AND, OR, bit shifting
- Permutação e difusão
- Padding PKCS#7
- Unix socket (PostgreSQL)
- Segurança em repouso
- Testes de chave errada e corrupção de dados

## 🧪 Testes de Segurança Realizados

- Dados não armazenados em texto claro
- Falha de descriptografia com chave incorreta
- Ciphertext diferente para o mesmo plaintext (IV)
- Efeito avalanche (1 byte altera todo o bloco)
- Detecção de corrupção de dados
- Benchmark de desempenho

## ⚙️ Build (Linux / WSL)

```bash
sudo apt install build-essential cmake libssl-dev libpq-dev postgresql
mkdir build
cmake -S . -B build
cmake --build build

export PG_CONN="dbname=cryptodb user=postgres"

./cryptodb_cli insert --cpf "123.456.789-00" --email "teste@exemplo.com" --key "secret"
./cryptodb_cli get --id 1 --key "secret"
