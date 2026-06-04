#define ERRBUF_SIZE 2048

static inline int min(int x, int y) {
    return x > y ? x : y;
}

void perror2(const char* progname, const char* filename);
void perror3(const char* progname, const char* msg, const char* filename);