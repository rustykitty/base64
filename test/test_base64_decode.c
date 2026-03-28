#include <stdlib.h>

int main() {
    if (system("base64 -w 0 tmp/test_data > encoded_data") != 0) {
        return 1;
    }

    // decoding time
    int core_retval = system("base64 -d tmp/encoded_data > coreutils.out");
    int custom_retval = system("../base64 -d tmp/encoded_data > custom.out");

    return 0;
}