SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=integral

all: 
	make lib

lib: integral.c
	$(CC) $(CFLAGS) integral.c -o $(EXEC)

test:
	echo "" > report.txt
	./integral 0.000000001 10 >> report.txt
	./integral 0.000000001 100 >> report.txt
	./integral 0.000000001 1000 >> report.txt
	cat report.txt

clean:
	find . -maxdepth 1 -not -name 'Makefile' -not -name 'integral.c' -delete 

	