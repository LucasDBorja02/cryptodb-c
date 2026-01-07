#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>
#include <stdint.h>

// Database connection and operations
PGconn* db_connect(const char *conninfo);
void db_disconnect(PGconn *conn);

// Store encrypted data in the database
int db_store_encrypted_data(PGconn *conn, const char *name,
                             const uint8_t *ciphertext, size_t ciphertext_len,
                             const uint8_t *iv);

// Retrieve encrypted data from the database
int db_retrieve_encrypted_data(PGconn *conn, const char *name,
                                uint8_t **ciphertext, size_t *ciphertext_len,
                                uint8_t **iv);

// List all stored records
int db_list_records(PGconn *conn);

#endif // DATABASE_H
