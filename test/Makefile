include ../Makefile.common

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_base64_encode: test_base64_encode.c

all: test_base64_encode
	./test_base64_encode