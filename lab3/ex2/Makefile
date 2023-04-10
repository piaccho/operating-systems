SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=my_ls

all: 
	make lib

lib: ls_lib.c
	$(CC) $(CFLAGS) ls_lib.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'test_dir_many' -not -name 'test_dir_empty' -not -name 'Makefile' -not -name 'ls_lib.c' -delete 

test:
	./$(EXEC) . 
	echo
	./$(EXEC) test_dir_empty
	echo
	./$(EXEC) test_dir_many
	