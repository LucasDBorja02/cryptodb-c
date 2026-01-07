#include "db/pg_store.h"
#include <libpq-fe.h>
#include <stdlib.h>
#include <string.h>

static int ensure_schema(PGconn* conn) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS secure_people ("
        "  id SERIAL PRIMARY KEY,"
        "  cpf_cipher BYTEA NOT NULL,"
        "  email_cipher BYTEA NOT NULL,"
        "  created_at TIMESTAMPTZ DEFAULT now()"
        ");";
    PGresult* r = PQexec(conn, sql);
    if (PQresultStatus(r) != PGRES_COMMAND_OK) { PQclear(r); return -1; }
    PQclear(r);
    return 0;
}

int pg_insert_secure_person(const char* conninfo,
                            const uint8_t* cpf_cipher, size_t cpf_len,
                            const uint8_t* email_cipher, size_t email_len,
                            int* out_id) {
    if (!conninfo || !cpf_cipher || !email_cipher || !out_id) return -1;

    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) { PQfinish(conn); return -2; }

    if (ensure_schema(conn) != 0) { PQfinish(conn); return -3; }

    const char* paramValues[2];
    int paramLengths[2];
    int paramFormats[2];

    paramValues[0] = (const char*)cpf_cipher;
    paramValues[1] = (const char*)email_cipher;
    paramLengths[0] = (int)cpf_len;
    paramLengths[1] = (int)email_len;
    paramFormats[0] = 1; // binary
    paramFormats[1] = 1; // binary

    PGresult* r = PQexecParams(conn,
        "INSERT INTO secure_people (cpf_cipher, email_cipher) VALUES ($1, $2) RETURNING id;",
        2, NULL, paramValues, paramLengths, paramFormats, 0);

    if (PQresultStatus(r) != PGRES_TUPLES_OK) {
        PQclear(r); PQfinish(conn); return -4;
    }

    char* idstr = PQgetvalue(r, 0, 0);
    *out_id = atoi(idstr);

    PQclear(r);
    PQfinish(conn);
    return 0;
}

int pg_get_secure_person(const char* conninfo, int id,
                         uint8_t** cpf_cipher, size_t* cpf_len,
                         uint8_t** email_cipher, size_t* email_len) {
    if (!conninfo || !cpf_cipher || !cpf_len || !email_cipher || !email_len) return -1;

    PGconn* conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK) { PQfinish(conn); return -2; }

    const char* paramValues[1];
    int paramLengths[1];
    int paramFormats[1];
    char idbuf[32];
    snprintf(idbuf, sizeof(idbuf), "%d", id);

    paramValues[0] = idbuf;
    paramLengths[0] = (int)strlen(idbuf);
    paramFormats[0] = 0; // text

    PGresult* r = PQexecParams(conn,
        "SELECT cpf_cipher, email_cipher FROM secure_people WHERE id = $1;",
        1, NULL, paramValues, paramLengths, paramFormats, 1); // ask binary results

    if (PQresultStatus(r) != PGRES_TUPLES_OK) { PQclear(r); PQfinish(conn); return -3; }
    if (PQntuples(r) == 0) { PQclear(r); PQfinish(conn); return -4; }

    int cpf_sz = PQgetlength(r, 0, 0);
    int email_sz = PQgetlength(r, 0, 1);

    uint8_t* cpf = (uint8_t*)malloc((size_t)cpf_sz);
    uint8_t* email = (uint8_t*)malloc((size_t)email_sz);
    if (!cpf || !email) { free(cpf); free(email); PQclear(r); PQfinish(conn); return -5; }

    memcpy(cpf, PQgetvalue(r, 0, 0), (size_t)cpf_sz);
    memcpy(email, PQgetvalue(r, 0, 1), (size_t)email_sz);

    *cpf_cipher = cpf;
    *cpf_len = (size_t)cpf_sz;
    *email_cipher = email;
    *email_len = (size_t)email_sz;

    PQclear(r);
    PQfinish(conn);
    return 0;
}
