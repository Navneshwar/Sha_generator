#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Rotate left operation
#define LEFTROTATE(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// SHA-0 Constants
static const uint32_t K[] = {
    0x5A827999, // 0-19
    0x6ED9EBA1, // 20-39
    0x8F1BBCDC, // 40-59
    0xCA62C1D6  // 60-79
};

// Initial hash values
static const uint32_t H0[] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0
};

// SHA-0 logical functions
#define F1(B, C, D) ((B & C) | ((~B) & D))
#define F2(B, C, D) (B ^ C ^ D)
#define F3(B, C, D) ((B & C) | (B & D) | (C & D))
#define F4(B, C, D) (B ^ C ^ D)

void sha0_transform(uint32_t state[5], const uint8_t block[64]) {
    uint32_t a, b, c, d, e, temp;
    uint32_t W[80];
    int t;

    // Initialize message schedule array
    for (t = 0; t < 16; t++) {
        W[t] = ((uint32_t)block[t*4] << 24) |
               ((uint32_t)block[t*4+1] << 16) |
               ((uint32_t)block[t*4+2] << 8) |
               ((uint32_t)block[t*4+3]);
    }

    // SHA-0 specific: No rotation in message schedule expansion
    for (t = 16; t < 80; t++) {
        W[t] = W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16];
    }

    // Initialize working variables
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // Main compression loop
    for (t = 0; t < 80; t++) {
        uint32_t f, k;

        if (t < 20) {
            f = F1(b, c, d);
            k = K[0];
        } else if (t < 40) {
            f = F2(b, c, d);
            k = K[1];
        } else if (t < 60) {
            f = F3(b, c, d);
            k = K[2];
        } else {
            f = F4(b, c, d);
            k = K[3];
        }

        temp = LEFTROTATE(a, 5) + f + e + k + W[t];
        e = d;
        d = c;
        c = LEFTROTATE(b, 30);
        b = a;
        a = temp;
    }

    // Update state
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

// Public interface function
char* sha0_hash(const char* input) {
    uint32_t state[5];
    uint8_t* message = NULL;
    uint64_t bit_len;
    size_t new_len, offset;
    int i;

    // Initialize hash values
    memcpy(state, H0, sizeof(H0));

    // Pre-processing: Padding
    bit_len = strlen(input) * 8;

    // Append '1' bit (0x80) and pad with zeros
    new_len = (((strlen(input) + 8) / 64) + 1) * 64;
    message = calloc(new_len + 64, 1);
    memcpy(message, input, strlen(input));
    message[strlen(input)] = 0x80;

    // Append length in bits as 64-bit big-endian
    for (i = 0; i < 8; i++) {
        message[new_len - 8 + i] = (bit_len >> (56 - i * 8)) & 0xFF;
    }

    // Process each 64-byte block
    for (offset = 0; offset < new_len; offset += 64) {
        sha0_transform(state, message + offset);
    }

    // Prepare output string (40 hex chars + null terminator)
    char* hash = malloc(41);
    snprintf(hash, 41, "%08x%08x%08x%08x%08x",
             state[0], state[1], state[2], state[3], state[4]);

    free(message);
    return hash;
}

// WASM-compatible interface
__attribute__((used)) const char* sha0_hash_wasm(const char* input) {
    return sha0_hash(input);
}