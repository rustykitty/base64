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
#endif

static struct options {
    bool decode;
} options = { .decode = false };

#define BLOCK_SIZE 32768

// size in bytes, NOT chunks
// return val: how many chunks encoded
size_t encode(char* const restrict out, const char* const restrict in, size_t size) {
    // how many chunks does the main loop process at once?
    static const size_t CHUNKS_PER_LOOP = 16;
    // input/output block size, based on our chunk size (16x)
    static const size_t IBS = CHUNKS_PER_LOOP * DECODED_CHUNK_SIZE; // 3x
    static const size_t OBS = CHUNKS_PER_LOOP * ENCODED_CHUNK_SIZE; // 4x

    // ceil(size / DECODED_CHUNK_SIZE)
    const size_t chunks = (size / DECODED_CHUNK_SIZE);
    const bool partial = size % DECODED_CHUNK_SIZE > 0;
    const size_t blocks = chunks / CHUNKS_PER_LOOP;
    const size_t remaining = chunks % CHUNKS_PER_LOOP;

    for (size_t i = 0; i < blocks; ++i) {
        encode_chunk_full_16x(out + (i * OBS), in + (i * IBS));
    }
    
    for (size_t i = 0; i < remaining; ++i) {
        encode_chunk_full(
            out + (blocks * OBS) + (i * ENCODED_CHUNK_SIZE),
            in + (blocks * IBS) + (i * DECODED_CHUNK_SIZE)
        );
    }
    if (partial) {
        encode_chunk(out + (blocks * OBS) + (remaining * ENCODED_CHUNK_SIZE), 
                     in + (blocks * IBS) + (remaining * DECODED_CHUNK_SIZE), 
                     size % DECODED_CHUNK_SIZE);
    }
    return chunks + partial;
}

int encode_stream(FILE* restrict from, FILE* restrict to) {
    static const size_t IBS = BLOCK_SIZE * DECODED_CHUNK_SIZE,
                        OBS = BLOCK_SIZE * ENCODED_CHUNK_SIZE;

    alignas(8) char in[IBS];
    alignas(8) char out[OBS];
    int read_ret;
    int write_ret;
    while ((read_ret = fread(in, 1, IBS, from)) > 0) {
        const size_t chunks = encode(out, in, read_ret);
        write_ret = fwrite(out, 1, chunks * ENCODED_CHUNK_SIZE, to);
        if (write_ret == EOF || (size_t)write_ret < chunks * ENCODED_CHUNK_SIZE) {
            if (ferror(to)) {
                goto error;
            }
        }
    }
    if (read_ret == EOF && ferror(from)) {
        goto error;
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
        } else {
            unreachable();
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
        FILE* stream = fopen(filename, "rb");
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
    }

    return 0;
error:
    perror2(progname, filename);
    return 1;
}