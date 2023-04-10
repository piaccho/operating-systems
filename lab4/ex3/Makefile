SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC1=catcher
EXEC2=sender

all: 
	make lib1
	make lib2

lib1: catcher.c
	$(CC) $(CFLAGS) catcher.c -o $(EXEC1)

lib2: sender.c
	$(CC) $(CFLAGS) sender.c -o $(EXEC2)

clean:
	find . -maxdepth 1 -not -name 'Makefile' -not -name 'sender.c' -not -name 'catcher.c' -delete 

	