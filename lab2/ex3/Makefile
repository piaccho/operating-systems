SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=wcdir

all: 
	make lib

lib: wc_dir.c
	$(CC) $(CFLAGS) wc_dir.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'test_dir' -not -name 'Makefile' -not -name 'wc_dir.c' -delete 

test:
	./$(EXEC)
	echo
	wc --bytes *