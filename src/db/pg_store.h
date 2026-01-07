#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Inserts into secure_people table. Returns inserted id (>0) on success.
int pg_insert_secure_person(const char* conninfo,
                            const uint8_t* cpf_cipher, size_t cpf_len,
                            const uint8_t* email_cipher, size_t email_len,
                            int* out_id);

// Fetches by id. Returns 0 on success and allocates cpf/email buffers (caller frees).
int pg_get_secure_person(const char* conninfo, int id,
                         uint8_t** cpf_cipher, size_t* cpf_len,
                         uint8_t** email_cipher, size_t* email_len);

#ifdef __cplusplus
}
#endif
