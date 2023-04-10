SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall

all: 
	make lib

lib: inverted_copy.c
	$(CC) $(CFLAGS) inverted_copy.c -o reverse

clean:
	find . -not -name 'input.txt' -not -name 'Makefile' -not -name 'inverted_copy.c' -delete

test:
	./reverse input.txt tmp1
	./reverse tmp1 tmp2 
	diff -s tmp2 input.txt