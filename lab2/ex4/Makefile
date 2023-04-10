SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC=wcdirrec

all: 
	make lib

lib: wc_dir_rec.c
	$(CC) $(CFLAGS) wc_dir_rec.c -o $(EXEC)

clean:
	find . -maxdepth 1 -not -name 'test_dir' -not -name 'Makefile' -not -name 'wc_dir_rec.c' -delete 

test:
	./$(EXEC) test_dir/
	echo 
	find test_dir/ | xargs wc --bytes
