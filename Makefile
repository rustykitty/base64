include Makefile.common

all: base64.o main

.PHONY: all test

base64.o: base64.c base64.h
	$(CC) $(CFLAGS) -c -o base64.o base64.c

base64_simd.o: base64_simd.c base64_simd.h

main: main.o base64.o base64_simd.o
	$(CC) $(CFLAGS) -o main main.o base64.o base64_simd.o $(LDFLAGS)

test: all
	make -C test