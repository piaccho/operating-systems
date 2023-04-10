SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC1=test_fork_exec
EXEC2=exec

all: 
	make lib1
	make lib2

lib1: test_fork_exec.c
	$(CC) $(CFLAGS) test_fork_exec.c -o $(EXEC1)
lib2: exec.c
	$(CC) $(CFLAGS) exec.c -o $(EXEC2)

clean:
	find . -maxdepth 1 -not -name 'raport2.txt' -not -name 'Makefile' -not -name 'test_fork_exec.c' -not -name 'exec.c' -delete 

test:
	./$(EXEC1) ignore 
	echo
	./$(EXEC1) handler
	echo
	./$(EXEC1) mask
	echo
	./$(EXEC1) pending
	echo
	./$(EXEC1) ignore exec
	echo
	./$(EXEC1) mask exec
	echo
	./$(EXEC1) pending exec