#include <stdio.h>
#include "base64.c"
// quick test
int main(void) {
    char* input = "000";
    char out[4];
    encode_block_full(out, input);
    // Should be MDAw
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "R";
    encode_block_partial(out, input, 1);
    // should be Ug==
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "AA";
    encode_block_partial(out, input, 2);
    // should be QUE=
    printf("%c%c%c%c\n", out[0], out[1], out[2], out[3]);

    input = "YWJj";
    decode_block_full(out, input);
    // Should be abc
    printf("%c%c%c\n", out[0], out[1], out[2]);

    // this is a shorter input- callers responsibility to clear output buf here
    input = "YXg=";
    memset(out, 0, 4);
    decode_block_padding(out, input);
    // Should be ax
    printf("%c%c%c\n", out[0], out[1], out[2]);
}