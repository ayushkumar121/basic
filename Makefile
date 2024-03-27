CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LIBS=-lm

.PHONY: build
build:
	$(CC) basic_test.c basic.c -o basic_test $(CFLAGS) $(LIBS)
