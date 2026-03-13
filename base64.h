#include <stdint.h>
#include <stddef.h>

void encode_block_full(char out[static 4], const char in[static 3]);
void encode_block_partial(char out[static 4], const char in[], int length);
void encode_block(char out[static 4], const char in[], int length);

void decode_block_full(char out[static 3], const char in[static 4]);
void decode_block_partial(char out[], const char in[], int length);
void decode_block(char out[], const char in[], int length);
void decode_block_padding(char out[static 3], const char in[static 4]);
