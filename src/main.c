#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cipher.h"
#include "crypto_utils.h"
#include "database.h"

#define MAX_INPUT_SIZE 4096

void print_usage(const char *prog_name) {
    printf("Usage: %s <command> [options]\n\n", prog_name);
    printf("Commands:\n");
    printf("  encrypt <name> <plaintext>  - Encrypt and store data\n");
    printf("  decrypt <name>              - Retrieve and decrypt data\n");
    printf("  list                        - List all stored records\n");
    printf("\n");
    printf("Environment variables:\n");
    printf("  DB_CONNINFO - PostgreSQL connection string (default: see code)\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s encrypt secret \"My secret message\"\n", prog_name);
    printf("  %s decrypt secret\n", prog_name);
    printf("  %s list\n", prog_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Get database connection info from environment or use default
    const char *conninfo = getenv("DB_CONNINFO");
    if (!conninfo) {
        conninfo = "host=localhost dbname=cryptodb user=postgres password=postgres";
    }
    
    // Connect to database
    PGconn *conn = db_connect(conninfo);
    if (!conn) {
        fprintf(stderr, "Failed to connect to database\n");
        return 1;
    }
    
    const char *command = argv[1];
    int result = 0;
    
    if (strcmp(command, "encrypt") == 0) {
        if (argc < 4) {
            fprintf(stderr, "Error: encrypt command requires name and plaintext\n");
            print_usage(argv[0]);
            db_disconnect(conn);
            return 1;
        }
        
        const char *name = argv[2];
        const char *plaintext = argv[3];
        size_t plaintext_len = strlen(plaintext);
        
        // Generate random key and IV
        uint8_t key[KEY_SIZE];
        uint8_t iv[IV_SIZE];
        
        if (generate_random_bytes(key, KEY_SIZE) != 0) {
            fprintf(stderr, "Failed to generate random key\n");
            db_disconnect(conn);
            return 1;
        }
        
        if (generate_random_bytes(iv, IV_SIZE) != 0) {
            fprintf(stderr, "Failed to generate random IV\n");
            db_disconnect(conn);
            return 1;
        }
        
        // Encrypt the plaintext
        uint8_t *ciphertext;
        size_t ciphertext_len;
        
        if (cbc_encrypt((const uint8_t *)plaintext, plaintext_len,
                        key, iv, &ciphertext, &ciphertext_len) != 0) {
            fprintf(stderr, "Encryption failed\n");
            db_disconnect(conn);
            return 1;
        }
        
        // Store in database
        if (db_store_encrypted_data(conn, name, ciphertext, ciphertext_len, iv) != 0) {
            fprintf(stderr, "Failed to store encrypted data\n");
            free(ciphertext);
            db_disconnect(conn);
            return 1;
        }
        
        // Display key for later decryption (in real app, this would be stored securely)
        char key_hex[KEY_SIZE * 2 + 1];
        bytes_to_hex(key, KEY_SIZE, key_hex);
        
        printf("Data encrypted and stored successfully!\n");
        printf("Name: %s\n", name);
        printf("Key (save this for decryption): %s\n", key_hex);
        printf("\nTo decrypt, run:\n");
        printf("  %s decrypt %s\n", argv[0], name);
        printf("  (when prompted, enter the key above)\n");
        
        free(ciphertext);
        
    } else if (strcmp(command, "decrypt") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: decrypt command requires name\n");
            print_usage(argv[0]);
            db_disconnect(conn);
            return 1;
        }
        
        const char *name = argv[2];
        
        // Prompt for key
        printf("Enter decryption key (64 hex characters): ");
        fflush(stdout);
        char key_hex[KEY_SIZE * 2 + 2]; // +2 for null terminator and newline
        if (fgets(key_hex, sizeof(key_hex), stdin) == NULL) {
            fprintf(stderr, "Failed to read key\n");
            db_disconnect(conn);
            return 1;
        }
        
        // Remove trailing newline if present
        size_t len = strlen(key_hex);
        if (len > 0 && key_hex[len - 1] == '\n') {
            key_hex[len - 1] = '\0';
            len--;
        }
        
        // Validate hex input
        if (len != KEY_SIZE * 2) {
            fprintf(stderr, "Invalid key length. Expected %d characters, got %zu\n", 
                    KEY_SIZE * 2, len);
            db_disconnect(conn);
            return 1;
        }
        
        for (size_t i = 0; i < KEY_SIZE * 2; i++) {
            if (!((key_hex[i] >= '0' && key_hex[i] <= '9') ||
                  (key_hex[i] >= 'a' && key_hex[i] <= 'f') ||
                  (key_hex[i] >= 'A' && key_hex[i] <= 'F'))) {
                fprintf(stderr, "Invalid hex character in key: '%c'\n", key_hex[i]);
                db_disconnect(conn);
                return 1;
            }
        }
        
        uint8_t key[KEY_SIZE];
        if (hex_to_bytes(key_hex, key, KEY_SIZE) != 0) {
            fprintf(stderr, "Invalid hex format in key\n");
            db_disconnect(conn);
            return 1;
        }
        
        // Retrieve encrypted data
        uint8_t *ciphertext;
        size_t ciphertext_len;
        uint8_t *iv;
        
        if (db_retrieve_encrypted_data(conn, name, &ciphertext, &ciphertext_len, &iv) != 0) {
            fprintf(stderr, "Failed to retrieve encrypted data\n");
            db_disconnect(conn);
            return 1;
        }
        
        // Decrypt the ciphertext
        uint8_t *plaintext;
        size_t plaintext_len;
        
        if (cbc_decrypt(ciphertext, ciphertext_len, key, iv,
                        &plaintext, &plaintext_len) != 0) {
            fprintf(stderr, "Decryption failed (wrong key or corrupted data)\n");
            free(ciphertext);
            free(iv);
            db_disconnect(conn);
            return 1;
        }
        
        printf("Decrypted data: %.*s\n", (int)plaintext_len, plaintext);
        
        free(ciphertext);
        free(iv);
        free(plaintext);
        
    } else if (strcmp(command, "list") == 0) {
        if (db_list_records(conn) != 0) {
            fprintf(stderr, "Failed to list records\n");
            result = 1;
        }
        
    } else {
        fprintf(stderr, "Unknown command: %s\n", command);
        print_usage(argv[0]);
        result = 1;
    }
    
    db_disconnect(conn);
    return result;
}
