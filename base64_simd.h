#include "base64.h"

/* hope your arrays are aligned */

int encode_chunk_full_2x(char out[static restrict 8], const char in_s[static restrict 6]);
int encode_chunk_full_4x(char out[static restrict 16], const char in_s[static restrict 12]);
int encode_chunk_full_8x(char out[static restrict 32], const char in_s[static restrict 24]);
int encode_chunk_full_16x(char out[static restrict 64], const char in_s[static restrict 48]);