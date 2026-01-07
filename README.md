# CryptoDB — Cryptography in C with PostgreSQL

CryptoDB é um projeto técnico de criptografia aplicada, desenvolvido em C, que integra um algoritmo criptográfico simétrico autoral com o PostgreSQL. O projeto demonstra, de forma prática, a implementação de criptografia de dados em repouso, garantindo que informações sensíveis sejam armazenadas apenas em formato cifrado no banco de dados, com a descriptografia ocorrendo exclusivamente na aplicação.

O foco do projeto está no estudo de fundamentos criptográficos, integração real com banco de dados e validação prática de propriedades de segurança, sendo voltado a fins educacionais, acadêmicos e demonstrativos.

---

## Objetivos do Projeto

- Implementar um algoritmo criptográfico simétrico autoral em linguagem C
- Aplicar criptografia de dados sensíveis antes da persistência em banco de dados
- Integrar criptografia com PostgreSQL utilizando armazenamento em formato BYTEA
- Validar propriedades criptográficas como confidencialidade, difusão e não-determinismo
- Comparar desempenho com algoritmos consolidados, como AES-256-CBC

---

## Funcionalidades

- Algoritmo criptográfico simétrico autoral baseado em cifragem por blocos
- Modo de operação CBC (Cipher Block Chaining) com IV aleatório
- Criptografia de campos sensíveis, como CPF e e-mail
- Armazenamento seguro de dados cifrados no PostgreSQL
- Descriptografia realizada exclusivamente na aplicação
- Testes de chave incorreta e detecção de corrupção de dados
- Benchmark comparativo com AES-256-CBC utilizando OpenSSL

---

## Conceitos Criptográficos Aplicados

- Operações bitwise (XOR, AND, OR)
- Deslocamento de bits (bit shifting)
- Permutação e difusão de dados
- Padding PKCS#7 para alinhamento de blocos
- Uso de IVs aleatórios para garantir não-determinismo
- Criptografia de dados em repouso
- Falha de descriptografia com chave incorreta

---

## Testes de Segurança Realizados

- Verificação de que dados sensíveis não são armazenados em texto claro no banco
- Falha de descriptografia ao utilizar chave incorreta
- Geração de ciphertext distinto para o mesmo plaintext
- Validação do efeito avalanche (pequenas alterações no plaintext geram grandes alterações no ciphertext)
- Detecção de falhas em caso de corrupção manual dos dados cifrados
- Análise de desempenho e comparação com AES-256-CBC

---

## Arquitetura Geral

O fluxo de funcionamento do sistema é descrito a seguir:

1. O dado sensível é fornecido à aplicação
2. O dado é criptografado utilizando o algoritmo autoral
3. O ciphertext é armazenado no PostgreSQL em formato BYTEA
4. Durante a consulta, o dado cifrado é recuperado
5. A descriptografia ocorre exclusivamente na aplicação

O banco de dados não possui acesso nem conhecimento sobre o conteúdo em texto claro.

---

## Build e Execução (Linux / WSL)

### Dependências

```bash
sudo apt install build-essential cmake libssl-dev libpq-dev postgresql

