SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall
EXEC1=client
EXEC2=server

all: 
	make client
	make server

client: $(EXEC1).c
	$(CC) $(CFLAGS) $(EXEC1).c -o $(EXEC1)


server: $(EXEC2).c
	$(CC) $(CFLAGS) $(EXEC2).c -o $(EXEC2)

test:
	echo test

clean:
	rm -f client server

	