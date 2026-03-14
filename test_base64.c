#include <stdio.h>
#include <string.h>
#include "base64.h"
// quick test
int main(void) {
    char input[3] = { 0x00, 0x00, 0x80 };
    char out[4];
    encode_chunk_full(out, input);
    // Should be AACA
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);
}