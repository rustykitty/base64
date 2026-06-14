#include "utility.h"

#if X86_64
#include <immintrin.h>
#endif

#include "base64.h"
#include "base64_simd.h"

// must link with base64.o
extern const char ALPHABET[64];
extern const unsigned char REVERSE_ALPHABET[256];
extern const char PADDING;

hot int encode_chunk_full_2x(char out[static restrict 8], const char in_s[static restrict 6]) {
    const unsigned char* restrict in = (const unsigned char* restrict) in_s;
    unsigned long long tmp = (unsigned long long)(in[0]) << 40 
                           | (unsigned long long)(in[1]) << 32
                           |      (unsigned long)(in[2]) << 24
                           |      (unsigned long)(in[3]) << 16
                           |      (unsigned long)(in[4]) << 8
                           | in[5];
#if X86_64
    uint64_t res =
        (uint64_t)ALPHABET[tmp >> 42 & 63] |
        (uint64_t)ALPHABET[tmp >> 36 & 63] << 8 |
        (uint64_t)ALPHABET[tmp >> 30 & 63] << 16 |
        (uint64_t)ALPHABET[tmp >> 24 & 63] << 24 |
        (uint64_t)ALPHABET[tmp >> 18 & 63] << 32 |
        (uint64_t)ALPHABET[tmp >> 12 & 63] << 40 |
        (uint64_t)ALPHABET[tmp >> 6 & 63] << 48 |
        (uint64_t)ALPHABET[tmp & 63] << 56;
    *(unsigned long long *)out = res;
#else
    encode_chunk_full(out, in_s);
    encode_chunk_full(out + 4, in_s + 3);
#endif
    return ENCODED_CHUNK_SIZE * 2;
}

hot int encode_chunk_full_4x(char out[static restrict 16], const char in_s[static restrict 12]) {
    encode_chunk_full_2x(out, in_s);
    encode_chunk_full_2x(out + 8, in_s + 6);
    return ENCODED_CHUNK_SIZE * 4;
}

hot int encode_chunk_full_8x(char out[static restrict 32], const char in_s[static restrict 24]) {
    encode_chunk_full_4x(out, in_s);
    encode_chunk_full_4x(out + 16, in_s + 12);
    return ENCODED_CHUNK_SIZE * 8;
}

hot int encode_chunk_full_16x(char out[static restrict 64], const char in_s[static restrict 48]) {
    encode_chunk_full_8x(out, in_s);
    encode_chunk_full_8x(out + 32, in_s + 24);
    return ENCODED_CHUNK_SIZE * 16;
}