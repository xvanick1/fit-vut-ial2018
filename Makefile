CC=gcc
CFLAGS=-std=c99 -Wall -pedantic

all: main

main: stack.o color.o
	$(CC) $(CFLAGS) -o main stack.o color.o 

color.o: color.c stack.h 
	$(CC) $(CFLAGS) -c color.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

clean: 
	$(RM) main *.o

