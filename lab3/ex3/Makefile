SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=find_rec

all: 
	make lib

lib: find_rec_lib.c
	$(CC) $(CFLAGS) find_rec_lib.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'test_dir' -not -name 'test_file_1' -not -name 'Makefile' -not -name 'find_rec_lib.c' -delete 

test:
	./$(EXEC) . alamakota
	echo
	./$(EXEC) "../.." "#include"
	