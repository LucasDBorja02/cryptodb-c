#include "database.h"
#include "crypto_utils.h"
#include "cipher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Connect to PostgreSQL database
PGconn* db_connect(const char *conninfo) {
    PGconn *conn = PQconnectdb(conninfo);
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    
    return conn;
}

// Disconnect from database
void db_disconnect(PGconn *conn) {
    if (conn) {
        PQfinish(conn);
    }
}

// Store encrypted data in the database
int db_store_encrypted_data(PGconn *conn, const char *name,
                             const uint8_t *ciphertext, size_t ciphertext_len,
                             const uint8_t *iv) {
    
    // Convert ciphertext and IV to hex strings
    char *ciphertext_hex = malloc(ciphertext_len * 2 + 1);
    char iv_hex[IV_SIZE * 2 + 1];
    
    if (!ciphertext_hex) {
        return -1;
    }
    
    bytes_to_hex(ciphertext, ciphertext_len, ciphertext_hex);
    bytes_to_hex(iv, IV_SIZE, iv_hex);
    
    // Check if record already exists
    const char *checkParams[1] = { name };
    PGresult *checkRes = PQexecParams(conn,
        "SELECT name FROM encrypted_data WHERE name = $1",
        1, NULL, checkParams, NULL, NULL, 0);
    
    int record_exists = (PQresultStatus(checkRes) == PGRES_TUPLES_OK && PQntuples(checkRes) > 0);
    PQclear(checkRes);
    
    // Prepare SQL statement based on whether record exists
    const char *paramValues[3] = { name, ciphertext_hex, iv_hex };
    PGresult *res;
    
    if (record_exists) {
        // Update existing record
        res = PQexecParams(conn,
            "UPDATE encrypted_data SET ciphertext = $2, iv = $3, created_at = CURRENT_TIMESTAMP WHERE name = $1",
            3, NULL, paramValues, NULL, NULL, 0);
        fprintf(stderr, "Warning: Updating existing record '%s'\n", name);
    } else {
        // Insert new record
        res = PQexecParams(conn,
            "INSERT INTO encrypted_data (name, ciphertext, iv) VALUES ($1, $2, $3)",
            3, NULL, paramValues, NULL, NULL, 0);
    }
    
    ExecStatusType status = PQresultStatus(res);
    PQclear(res);
    free(ciphertext_hex);
    
    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "Insert failed: %s\n", PQerrorMessage(conn));
        return -1;
    }
    
    return 0;
}

// Retrieve encrypted data from the database
int db_retrieve_encrypted_data(PGconn *conn, const char *name,
                                uint8_t **ciphertext, size_t *ciphertext_len,
                                uint8_t **iv) {
    
    const char *paramValues[1] = { name };
    
    PGresult *res = PQexecParams(conn,
        "SELECT ciphertext, iv FROM encrypted_data WHERE name = $1",
        1, NULL, paramValues, NULL, NULL, 0);
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Select failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    if (PQntuples(res) == 0) {
        fprintf(stderr, "No data found for name: %s\n", name);
        PQclear(res);
        return -1;
    }
    
    // Get hex strings from result
    const char *ciphertext_hex = PQgetvalue(res, 0, 0);
    const char *iv_hex = PQgetvalue(res, 0, 1);
    
    // Calculate ciphertext length
    *ciphertext_len = strlen(ciphertext_hex) / 2;
    
    // Allocate buffers
    *ciphertext = malloc(*ciphertext_len);
    *iv = malloc(IV_SIZE);
    
    if (!*ciphertext || !*iv) {
        free(*ciphertext);
        free(*iv);
        PQclear(res);
        return -1;
    }
    
    // Convert hex to bytes
    hex_to_bytes(ciphertext_hex, *ciphertext, *ciphertext_len);
    hex_to_bytes(iv_hex, *iv, IV_SIZE);
    
    PQclear(res);
    return 0;
}

// List all stored records
int db_list_records(PGconn *conn) {
    PGresult *res = PQexec(conn,
        "SELECT name, created_at FROM encrypted_data ORDER BY created_at DESC");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Select failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    
    int ntuples = PQntuples(res);
    
    if (ntuples == 0) {
        printf("No records found.\n");
    } else {
        printf("Stored records:\n");
        printf("%-30s %s\n", "Name", "Created At");
        printf("%-30s %s\n", "----", "----------");
        
        for (int i = 0; i < ntuples; i++) {
            const char *name = PQgetvalue(res, i, 0);
            const char *created_at = PQgetvalue(res, i, 1);
            printf("%-30s %s\n", name, created_at);
        }
    }
    
    PQclear(res);
    return 0;
}
