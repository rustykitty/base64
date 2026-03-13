#include <stdio.h>
#include <string.h>
#include "base64.h"
// quick test
int main(void) {
    char* input = "000";
    char out[4];
    encode_chunk_full(out, input);
    // Should be MDAw
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "R";
    encode_chunk_partial(out, input, 1);
    // should be Ug==
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "AA";
    encode_chunk_partial(out, input, 2);
    // should be QUE=
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "YWJj";
    decode_chunk_full(out, input);
    // Should be abc
    printf("%c%c%c\n", out[0], out[1], out[2]);

    // this is a shorter input- callers responsibility to clear output buf here
    input = "YXg=";
    memset(out, 0, 4);
    decode_chunk_padding(out, input);
    // Should be ax
    printf("%c%c%c\n", out[0], out[1], out[2]);
}