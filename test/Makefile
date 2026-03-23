include ../Makefile.common

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


all: test_base64_encode
	./test_base64_encode

test_base64_encode: test_base64_encode.c

.PHONY: all

.DEFAULT_GOAL = all