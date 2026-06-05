#define ERRBUF_SIZE 2048

// restrict
#if __STDC_VERSION__ >= 199901L
#define restrict restrict
#elif defined(__GNUC__)
#define restrict __restrict__
#elif defined(_MSC_VER)
#define restrict __restrict
#else
#define restrict
#endif

// alignas
#if __STDC_VERSION__ >= 202311L
#define alignas(_x) alignas(_x)
#elif __STDC_VERSION__ >= 201112L
#define alignas(_x) _Alignas(_x)
#elif defined(__GNUC__)
#define alignas(_x) __attribute__ ((aligned (_x)))
#elif defined(_MSC_VER)
#define alignas(_x) __declspec(align(_x))
#endif

#ifdef __GNUC__
#define hot __attribute__((hot))
#else
#define hot
#endif

static inline int min(int x, int y) {
    return x > y ? x : y;
}

void perror2(const char* progname, const char* filename);
void perror3(const char* progname, const char* msg, const char* filename);