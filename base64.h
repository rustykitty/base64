#include <stdint.h>
#include <stddef.h>

int encode_chunk_full(char out[static 4], const char in[static 3]);
int encode_chunk_partial(char out[static 4], const char in[], int length);
int encode_chunk(char out[static 4], const char in[], int length);

int decode_chunk_full(char out[static 3], const char in[static 4]);
int decode_chunk_partial(char out[], const char in[], int length);
int decode_chunk(char out[], const char in[], int length);
int decode_chunk_padding(char out[static 3], const char in[static 4]);
