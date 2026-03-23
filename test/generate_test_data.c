#include <stdio.h>
#include <errno.h>

int main() {
        // write to a file
    FILE* out = fopen("tmp/test_data", "w");
    if (!out) {
        perror("Failed to open/create tmp/test_data");
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
}