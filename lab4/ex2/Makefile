SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=test_flags

all: 
	make lib

lib: test_flags.c
	$(CC) $(CFLAGS) test_flags.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'Makefile' -not -name 'test_flags.c' -delete 

	