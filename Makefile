CC ?= gcc

all: base64.o test_base64

.PHONY: all

base64.o: base64.c base64.h
	$(CC) $(CFLAGS) -c -o base64.o base64.c

test_base64.o: test_base64.c
	$(CC) $(CFLAGS) -c -o test_base64.o test_base64.c

test_base64: test_base64.o base64.o
	$(CC) $(CFLAGS) -o test_base64 test_base64.o base64.o $(LDFLAGS)
