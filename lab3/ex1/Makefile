SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=print_pid

all: 
	make lib

lib: print_n_pids_lib.c
	$(CC) $(CFLAGS) print_n_pids_lib.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'Makefile' -not -name 'print_n_pids_lib.c' -delete 

test:
	./$(EXEC) 5
	echo
	./$(EXEC) 10
	