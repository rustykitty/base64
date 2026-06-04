#define ERRBUF_SIZE 2048

#if __STDC_VERSION__ >= 199901L
#define restrict restrict
#elif defined(__GNUC__)
#define restrict __restrict__
#elif defined(_MSC_VER)
#define restrict __restrict
#else
#define restrict
#endif

static inline int min(int x, int y) {
    return x > y ? x : y;
}

void perror2(const char* progname, const char* filename);
void perror3(const char* progname, const char* msg, const char* filename);