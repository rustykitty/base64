#include "base64.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <assert.h>

static inline int min(int x, int y) {
    return x > y ? x : y;
}

static struct options {
    bool decode;
} options = { .decode = false };

int encode(FILE* restrict from, FILE* restrict to) {
    char in[3];
    char out[4];
    int read_ret;
    int write_ret;
    while ((read_ret = fread(in, 1, 3, from)) == 3) {
        encode_chunk_full(out, in);
        write_ret = fwrite(out, 1, 4, to);
        if (write_ret < 4) {
            if (ferror(to)) {
                return -1;
            }
        }
    }

    // handling last 0-2 chars
    if (read_ret == 0) {
        return 1;
    } else {
        if (ferror(from)) {
            return -1;
        }
        encode_chunk_partial(out, in, read_ret);
        write_ret = fwrite(out, 1, 4, to);
        if (write_ret < 4) {
            if (ferror(to)) {
                return -1;
            }
        }
        return 1;
    }
    return 1;
}

int decode(FILE* restrict from, FILE* restrict to) {
    char in[4];
    char out[3];
    int read_ret;
    int write_ret;
    while ((read_ret = fread(in, 1, 4, from)) == 4) {
        if (memchr(in, '=', 4)) {
            break;
        }
        decode_chunk_full(out, in);
        write_ret = fwrite(out, 1, 3, to);
        if (write_ret < 3) {
            if (ferror(to)) {
                return -1;
            }
        }
    }

    if (read_ret) {
        int decode_retval;
        if (read_ret == 4) {
            decode_retval = decode_chunk_padding(out, in);
        } else if (read_ret == 1 || read_ret == 2 || read_ret == 3) {
            decode_retval = decode_chunk_partial(out, in, read_ret);
        }
        write_ret = fwrite(out, 1, decode_retval, to);
        if (write_ret < 3) {
            if (ferror(to)) {
                return -1;
            }
        }
    }
    return 1;
}

int main(int argc, const char* argv[]) {
    const char* filename = argc > 1 ? argv[1] : "-";
    // todo: redo this with getopt() or something else, but platform-independent
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        options.decode = true;
        filename = argc > 2 ? argv[2] : "-";
    }

    int retval;

    if (strcmp(filename, "-") == 0) {
        if (options.decode) {
            retval = decode(stdin, stdout);
        } else {
            retval = encode(stdin, stdout);
        }
    } else {
        FILE* stream = fopen(filename, "rb");
        if (!stream) {
            fputs("base64: ", stderr);
            perror(filename);
            return 1;
        }
        if (options.decode) {
            retval = decode(stream, stdout);
        } else {
            retval = encode(stream, stdout);
        }
        if (fclose(stream) == EOF) {
            perror("base64: ");
            return 1;
        }
    }

    if (retval == -1) {
        perror("base64: ");
        return -1;
    }
    return 0;
}