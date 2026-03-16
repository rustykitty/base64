#include "base64.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <assert.h>

static inline int min(int x, int y) {
    return x > y ? x : y;
}

#define ERRBUF_SIZE 2048

// perror() but with progname and filename
// e.g. base64: /nonexistent: No such file or directory
// max 4095 bytes!
void perror2(const char* progname, const char* filename) {
    const char buf[ERRBUF_SIZE * 2] = {};
    snprintf(buf, ERRBUF_SIZE * 2 - 1, "%s: %s", progname, filename);
    perror(buf);
}

// perror() with three components
// e.g. base64: can't stat: somefile: Input/output error
void perror3(const char* progname, const char* msg, const char* filename) {
    const char buf[ERRBUF_SIZE * 3] = {};
    snprintf(buf, ERRBUF_SIZE * 3 - 1, "%s: %s: %s", progname, msg, filename);
    perror(buf);
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
                goto error;
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
                goto error;
            }
        }
        return 1;
    }
    return 1;
error:
    perror2(progname, "Error while encoding file");
    return -1;
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
                goto error;
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
                goto error;
            }
        }
    }
    return 1;
error:
    perror2(progname, "Error while decoding file");
    return -1;
}

static const char* progname = "base64";

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
            goto error;
        }
        if (options.decode) {
            retval = decode(stream, stdout);
        } else {
            retval = encode(stream, stdout);
        }
        if (retval == -1) {
            goto error;
        }
        if (fclose(stream) == EOF) {
            goto error;
        }
    }

    return 0;
error:
    perror2(progname, filename);
    return 1;
}