#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto/xorfeistel.h"
#include "crypto/cbc.h"
#include "db/pg_store.h"
#include "util/secure_mem.h"

static const char* env_or(const char* key, const char* fallback) {
    const char* v = getenv(key);
    return v ? v : fallback;
}

static void usage(void) {
    puts("CryptoDB CLI (educational)\n"
         "Commands:\n"
         "  insert --cpf <str> --email <str> --key <pass>\n"
         "  get    --id <int>  --key <pass>\n"
         "\nEnv:\n"
         "  PG_CONN  PostgreSQL conninfo string\n");
}

static int arg_eq(const char* a, const char* b) { return a && b && strcmp(a,b)==0; }

int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 1; }
    const char* cmd = argv[1];

    const char* conninfo = env_or("PG_CONN", NULL);
    if (!conninfo) {
        fprintf(stderr, "ERROR: set PG_CONN env var (PostgreSQL conninfo).\n");
        return 2;
    }

    if (arg_eq(cmd, "insert")) {
        const char* cpf = NULL;
        const char* email = NULL;
        const char* key = NULL;

        for (int i = 2; i < argc; i++) {
            if (arg_eq(argv[i], "--cpf") && i+1 < argc) cpf = argv[++i];
            else if (arg_eq(argv[i], "--email") && i+1 < argc) email = argv[++i];
            else if (arg_eq(argv[i], "--key") && i+1 < argc) key = argv[++i];
        }
        if (!cpf || !email || !key) { usage(); return 3; }

        xfs_ctx_t ctx;
        if (xfs_init(&ctx, (const uint8_t*)key, strlen(key), 16) != 0) {
            fprintf(stderr, "ERROR: xfs_init failed\n");
            return 4;
        }

        uint8_t* cpf_ct = NULL; size_t cpf_ct_len = 0;
        uint8_t* email_ct = NULL; size_t email_ct_len = 0;

        if (xfs_cbc_encrypt(&ctx, (const uint8_t*)cpf, strlen(cpf), NULL, &cpf_ct, &cpf_ct_len) != 0 ||
            xfs_cbc_encrypt(&ctx, (const uint8_t*)email, strlen(email), NULL, &email_ct, &email_ct_len) != 0) {
            fprintf(stderr, "ERROR: encryption failed\n");
            free(cpf_ct); free(email_ct);
            return 5;
        }

        int id = 0;
        if (pg_insert_secure_person(conninfo, cpf_ct, cpf_ct_len, email_ct, email_ct_len, &id) != 0) {
            fprintf(stderr, "ERROR: DB insert failed\n");
            free(cpf_ct); free(email_ct);
            return 6;
        }

        printf("Inserted id=%d\n", id);

        // Clear sensitive buffers
        secure_bzero(&ctx, sizeof(ctx));
        secure_bzero(cpf_ct, cpf_ct_len);
        secure_bzero(email_ct, email_ct_len);
        free(cpf_ct); free(email_ct);
        return 0;
    }

    if (arg_eq(cmd, "get")) {
        const char* key = NULL;
        int id = -1;

        for (int i = 2; i < argc; i++) {
            if (arg_eq(argv[i], "--key") && i+1 < argc) key = argv[++i];
            else if (arg_eq(argv[i], "--id") && i+1 < argc) id = atoi(argv[++i]);
        }
        if (!key || id <= 0) { usage(); return 3; }

        uint8_t* cpf_ct = NULL; size_t cpf_ct_len = 0;
        uint8_t* email_ct = NULL; size_t email_ct_len = 0;

        if (pg_get_secure_person(conninfo, id, &cpf_ct, &cpf_ct_len, &email_ct, &email_ct_len) != 0) {
            fprintf(stderr, "ERROR: DB get failed (id=%d)\n", id);
            return 4;
        }

        xfs_ctx_t ctx;
        if (xfs_init(&ctx, (const uint8_t*)key, strlen(key), 16) != 0) {
            fprintf(stderr, "ERROR: xfs_init failed\n");
            free(cpf_ct); free(email_ct);
            return 5;
        }

        uint8_t* cpf_pt = NULL; size_t cpf_pt_len = 0;
        uint8_t* email_pt = NULL; size_t email_pt_len = 0;

        if (xfs_cbc_decrypt(&ctx, cpf_ct, cpf_ct_len, &cpf_pt, &cpf_pt_len) != 0 ||
            xfs_cbc_decrypt(&ctx, email_ct, email_ct_len, &email_pt, &email_pt_len) != 0) {
            fprintf(stderr, "ERROR: decryption failed (wrong key?)\n");
            free(cpf_ct); free(email_ct);
            free(cpf_pt); free(email_pt);
            return 6;
        }

        printf("id=%d\ncpf=%s\nemail=%s\n", id, (char*)cpf_pt, (char*)email_pt);

        secure_bzero(&ctx, sizeof(ctx));
        secure_bzero(cpf_ct, cpf_ct_len);
        secure_bzero(email_ct, email_ct_len);
        secure_bzero(cpf_pt, cpf_pt_len);
        secure_bzero(email_pt, email_pt_len);

        free(cpf_ct); free(email_ct);
        free(cpf_pt); free(email_pt);
        return 0;
    }

    usage();
    return 1;
}
