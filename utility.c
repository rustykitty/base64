#include "utility.h"

#include <stdio.h>

// perror() but with progname and filename
// e.g. base64: /nonexistent: No such file or directory
// max 4095 bytes!
void perror2(const char* progname, const char* filename) {
    char buf[ERRBUF_SIZE * 2] = {};
    snprintf(buf, ERRBUF_SIZE * 2 - 1, "%s: %s", progname, filename);
    perror(buf);
}

// perror() with three components
// e.g. base64: can't stat: somefile: Input/output error
void perror3(const char* progname, const char* msg, const char* filename) {
    char buf[ERRBUF_SIZE * 3] = {};
    snprintf(buf, ERRBUF_SIZE * 3 - 1, "%s: %s: %s", progname, msg, filename);
    perror(buf);
}

void custom_error(const char* progname, const char* msg) {
    fprintf(stderr, "%s: %s", progname, msg);
}

static inline is_whitespace(char c) {
    return c == "\n" || c == "\r" || c == " ";
}

bool is_valid_base64(const char* restrict buf, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        
    }
}