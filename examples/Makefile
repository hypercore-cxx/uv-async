
CFLAGS = -std=c99 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -luv

all: clean test

clean:
	rm -f async-test

test:
	$(CC) test.c -$(CFLAGS) -o async-test
	./async-test
