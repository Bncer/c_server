CC=gcc
CFLAGS=-I.

compile: server.c
	$(CC) server.c -o server $(CFLAGS)
