#if defined(__x86_64__) || defined(_M_X64)
#define X86_64 1
#include <immintrin.h>
#endif

#include "base64.h"

#include "base64_simd.h"

// must link with base64.o
extern const char ALPHABET[64];
extern const unsigned char REVERSE_ALPHABET[256];
extern const char PADDING;

// see base64.c
#define _ENCODE(_x) ((_x) < 26 ? (_x) + 'A' : \
                    ((_x) < 52 ? ((_x) - 26) + 'a' : \
                    ((_x) < 62 ? ((_x) - 52) + '0' : \
                    ((_x) == 62 ?            '+' : \
                     ('/'))))) /* last one is 63 */

#define ENCODE(_x) _ENCODE((unsigned char)(_x) & 63)


int encode_chunk_full_2x(char out[static restrict 8], const char in_s[static restrict 6]) {
#if X86_64
    const unsigned char* restrict in = (const unsigned char* restrict) in_s;
    unsigned long long tmp = (unsigned long long)(in[0]) << 40 
                           | (unsigned long long)(in[1]) << 32
                           | (unsigned long long)(in[2]) << 24
                           | (unsigned long long)(in[3]) << 16
                           | (unsigned long long)(in[4]) << 8
                           | in[5];
    out[0] = ENCODE(tmp >> 42 & 63);
    out[1] = ENCODE(tmp >> 36 & 63);
    out[2] = ENCODE(tmp >> 30 & 63);
    out[3] = ENCODE(tmp >> 24 & 63);
    out[4] = ENCODE(tmp >> 18 & 63);
    out[5] = ENCODE(tmp >> 12 & 63);
    out[6] = ENCODE(tmp >> 6 & 63);
    out[7] = ENCODE(tmp & 63);
#else
    encode_chunk_full(out, in_s);
    encode_chunk_full(out + 4, in_s + 3);
#endif
    return OUTPUT_CHUNK_SIZE * 2;
}

int encode_chunk_full_4x(char out[static restrict 16], const char in_s[static restrict 12]) {
    encode_chunk_full_2x(out, in_s);
    encode_chunk_full_2x(out + 8, in_s + 6);
    return OUTPUT_CHUNK_SIZE * 4;
}

int encode_chunk_full_8x(char out[static restrict 32], const char in_s[static restrict 24]) {
    encode_chunk_full_4x(out, in_s);
    encode_chunk_full_4x(out + 16, in_s + 12);
    return OUTPUT_CHUNK_SIZE * 8;
}

int encode_chunk_full_16x(char out[static restrict 64], const char in_s[static restrict 48]) {
    encode_chunk_full_8x(out, in_s);
    encode_chunk_full_8x(out + 32, in_s + 24);
    return OUTPUT_CHUNK_SIZE * 16;
}