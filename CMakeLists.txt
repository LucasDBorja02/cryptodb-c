cmake_minimum_required(VERSION 3.16)
project(cryptodb C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

add_library(cryptodb_lib
    src/crypto/xorfeistel.c
    src/crypto/padding.c
    src/crypto/cbc.c
    src/crypto/aes_openssl.c
    src/util/hex.c
    src/util/secure_mem.c
    src/db/pg_store.c
)
target_include_directories(cryptodb_lib PUBLIC src)
target_link_libraries(cryptodb_lib PUBLIC OpenSSL::Crypto PostgreSQL::PostgreSQL)

add_executable(cryptodb_cli tools/cryptodb_cli.c)
target_link_libraries(cryptodb_cli PRIVATE cryptodb_lib)

add_executable(cryptodb_bench tools/cryptodb_bench.c)
target_link_libraries(cryptodb_bench PRIVATE cryptodb_lib)
