SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC1=integral
EXEC2=compute

all: 
	make lib1
	make lib2

lib1: integral.c
	$(CC) $(CFLAGS) integral.c -o $(EXEC1)

lib2: compute.c
	$(CC) $(CFLAGS) compute.c -o $(EXEC2)

test:
	echo "" > report.txt
	./integral 0.00000001 10 >> report.txt
	./integral 0.000000001 10 >> report.txt
	./integral 0.000000001 50 >> report.txt
	./integral 0.000000001 100 >> report.txt
	cat report.txt

clean:
	find . -maxdepth 1 -not -name 'Makefile' -not -name 'compute.c' -not -name 'integral.c' -delete 

	