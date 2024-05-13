CC=clang
CFLAGS=-Wall -Werror -Wextra -Wpedantic -g
LIBS=-lm

.PHONY: build
basic_test: basic.c basic_test.c
	$(CC) -o basic_test basic_test.c basic.c $(CFLAGS) $(LIBS)
