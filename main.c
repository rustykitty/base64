#include "base64.h"
#include "base64_simd.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <assert.h>

static const char* progname = "base64";
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#include <unistd.h> // _POSIX_VERSION
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif

static struct options {
    bool decode;
} options = { .decode = false };

int encode_stream(FILE* restrict from, FILE* restrict to) {
    char in[48];
    char out[64];
    int read_ret;
    int write_ret;
    while ((read_ret = fread(in, 1, 48, from)) == 48) {
        encode_chunk_full_16x(out, in);
        write_ret = fwrite(out, 1, 64, to);
        if (write_ret < 64) {
            if (ferror(to)) {
                goto error;
            }
        }
    }

    // handling last couple rounds
    while (read_ret > 3) {
        encode_chunk_full(out, in);
        write_ret = fwrite(out, 1, 4, to);
        if (write_ret < 4) {
            if (ferror(to)) {
                goto error;
            }
        }
        read_ret -= 3;
    }
    if (read_ret > 0) {
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
    }
    return 1;
error:
    perror2(progname, "Error while encoding file");
    return -1;
}

int decode_stream(FILE* restrict from, FILE* restrict to) {
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

// Linux only-- too bad!
int encode_memory(const char* const restrict from_signed, FILE* restrict to, size_t length) {
    const char* restrict from = from_signed;
    char out[64];
    int write_ret;
    size_t full_blocks = length / 48;
    int partial_size = length % 48;
    for (size_t i = 0; i < full_blocks; ++i) {
        encode_chunk_full_16x(out, &from[i * 48]);
        write_ret = fwrite(out, 1, 64, to);
        if (write_ret < 64) {
            if (ferror(to)) {
                return -1;
            }
        }
    }
    const char* restrict p = from + full_blocks * 48;
    while (partial_size > 3) {
        encode_chunk_full(out, p);
        write_ret = fwrite(out, 1, 4, to);
        if (write_ret < 4) {
            if (ferror(to)) {
                return -1;
            }
        }
        p += 3;
        partial_size -= 3;
    }
    if (partial_size > 0) {
        encode_chunk_partial(out, p, partial_size);
        write_ret = fwrite(out, 1, 4, to);
        if (write_ret < 4) {
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
            retval = decode_stream(stdin, stdout);
        } else {
            retval = encode_stream(stdin, stdout);
        }
        if (retval == -1) {
            goto error;
        }
    } else {
#ifdef _POSIX_VERSION
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            fputs("Call to open failed: ", stderr);
            goto error;
        }
        struct stat stat;
        int stat_retval = fstat(fd, &stat);
        if (stat_retval == -1) {
            fputs("Can't stat ", stderr);
            goto error;
        }
        const char* buf = (const char*) mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (buf == MAP_FAILED) {
            perror("mmap failed");
            return -1;
        }
        if (options.decode) {
            // todo: implement decode_memory
            FILE* stream = fmemopen((void*)buf, stat.st_size, "r");
            if (!stream) {
                goto error;
            }
            decode_stream(stream, stdout);
        } else {
            encode_memory(buf, stdout, stat.st_size);
        }
#else
        const char* mode = options.decode ? "r" : "rb";
        FILE* stream = fopen(filename, mode);
        if (!stream) {
            goto error;
        }
        if (options.decode) {
            retval = decode_stream(stream, stdout);
        } else {
            retval = encode_stream(stream, stdout);
        }
        if (retval == -1) {
            goto error;
        }
        if (fclose(stream) == EOF) {
            goto error;
        }
#endif
    }

    return 0;
error:
    perror2(progname, filename);
    return 1;
}