CFLAGS = -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -L/usr/local/lib -luv -pthread

all: clean test

clean:
	rm -f async-test

test:
	$(CC) test.c $(CFLAGS) -o async-test
	LD_LIBRARY_PATH=/usr/local/lib ./async-test
