SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall

all: 
	make lib

lib: replace_char_lib.c
	$(CC) $(CFLAGS) replace_char_lib.c -o zamien

clean:
	find . -not -name 'input.txt' -not -name 'Makefile' -not -name 'replace_char_lib.c' -delete

test:
	./zamien a 0 input.txt output.txt
	tr a 0 < input.txt > tmp
	diff -s tmp output.txt