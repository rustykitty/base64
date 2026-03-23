#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
// only works on posix but oh well
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char* argv[]) {
    // cd "$(dirname "$0")"
    chdir(dirname(argv[0]));

    // write to a file
    FILE* out = fopen("test_data", "w");
    if (!out) {
        perror("Failed to open ./test_data");
    }
    // all 3-byte possibilities
    for (int i = 0; i < 256; ++i) {
        for (int j = 0; j < 256; ++j) {
            for (int k = 0; k < 256; ++k) {
                char buf[3] = { i, j, k };
                int retval = fwrite(buf, 3, 1, out);
                if (!retval) {
                    perror("Error occurred while writing data");
                }
            }
        }
    }
    fputc(0, out); // this tests padding
    fflush(out);
    fclose(out);

    // coreutils base64
    // -w 0 disables wrapping
    int coreutils_retval = system("base64 test_data -w 0 > coreutils.out");
    int custom_retval = system("../main test_data > custom.out");

    if (coreutils_retval != 0 || custom_retval != 0) {
        return 1;
    }

    // compare files
    int core_fd = open("coreutils.out", O_RDONLY);
    int custom_fd = open("custom.out", O_RDONLY);

    if (core_fd == -1) {
        perror("Failed to open coreutils.out");
    } else if (custom_fd == -1) {
        perror("Failed to open custom.out");
    }

    struct stat core_stat, custom_stat;
    if (fstat(core_fd, &core_stat) == -1) {
        fprintf(stderr, "Can't stat %s", "coreutils.out");
        perror("");
    }
    if (fstat(custom_fd, &custom_stat) == -1) {
        fprintf(stderr, "Can't stat %s", "custom.out");
        perror("");
    }

    if (core_stat.st_size != custom_stat.st_size) {
        printf(
            "Different size- GNU coreutils `base64` has %ld bytes, custom version has %ld bytes",
            core_stat.st_size, custom_stat.st_size
        );
        return 1;
    }

    const char* core_mem = (const char*) mmap(NULL, core_stat.st_size, PROT_READ, MAP_PRIVATE, core_fd, 0);

    if (core_mem == MAP_FAILED) {
        perror("");
    }

    const char* custom_mem = (const char*) mmap(NULL, custom_stat.st_size, PROT_READ, MAP_PRIVATE, custom_fd, 0);

    if (custom_mem == MAP_FAILED) {
        perror("");
    }

    // not using memcmp because it doesn't tell us where they aren't equal
    size_t length = core_stat.st_size;
    size_t chunks = length / 4;
    for (size_t i = 0; i < chunks; ++i) {
        const char* core_chunk = &core_mem[i * 4];
        const char* custom_chunk = &custom_mem[i * 4];
        if (memcmp(core_chunk, custom_chunk, 4) != 0) {
            printf("chunk %zu (at %zu) differs -- core `%.4s`, custom `%.4s`\n", i, i * 4, core_chunk, custom_chunk);
            return 1;
        }
    }

}