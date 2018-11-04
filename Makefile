CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic

all: main

main: stack.o color.o
	$(CC) $(CFLAGS) -o main stack.o color.o 

color.o: color.c stack.h 
	$(CC) $(CFLAGS) -c color.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

clean-eva:
	rm -r color.o stack.o main

clean: 
	$(RM) main *.o

