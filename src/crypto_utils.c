#include "crypto_utils.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// Generate random bytes using /dev/urandom
int generate_random_bytes(uint8_t *buffer, size_t length) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    
    ssize_t bytes_read = read(fd, buffer, length);
    close(fd);
    
    if (bytes_read != (ssize_t)length) {
        return -1;
    }
    
    return 0;
}

// Add PKCS7 padding
size_t add_pkcs7_padding(uint8_t *data, size_t data_len, size_t block_size) {
    size_t padding_len = block_size - (data_len % block_size);
    
    for (size_t i = 0; i < padding_len; i++) {
        data[data_len + i] = (uint8_t)padding_len;
    }
    
    return data_len + padding_len;
}

// Remove PKCS7 padding
int remove_pkcs7_padding(uint8_t *data, size_t data_len, size_t *unpadded_len) {
    if (data_len == 0) {
        return -1;
    }
    
    uint8_t padding_len = data[data_len - 1];
    
    // Validate padding
    if (padding_len == 0 || padding_len > data_len) {
        return -1;
    }
    
    // Check all padding bytes
    for (size_t i = data_len - padding_len; i < data_len; i++) {
        if (data[i] != padding_len) {
            return -1;
        }
    }
    
    *unpadded_len = data_len - padding_len;
    return 0;
}

// Convert bytes to hex string
void bytes_to_hex(const uint8_t *bytes, size_t len, char *hex_str) {
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_str + (i * 2), "%02x", bytes[i]);
    }
    hex_str[len * 2] = '\0';
}

// Convert hex string to bytes
void hex_to_bytes(const char *hex_str, uint8_t *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        sscanf(hex_str + (i * 2), "%2hhx", &bytes[i]);
    }
}
