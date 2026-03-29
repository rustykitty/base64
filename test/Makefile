include ../Makefile.common

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)


all: test_base64_encode generate_test_data
	mkdir -p tmp
	./generate_test_data
	./test_base64_encode

generate_test_data: generate_test_data.c

test_base64_encode: test_base64_encode.c

clean:
	rm -rf tmp
	rm -f generate_test_data test_base64_encode

.PHONY: all clean

.DEFAULT_GOAL = all