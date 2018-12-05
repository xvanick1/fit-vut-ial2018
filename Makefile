CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic

all: helpers.o main.o
	$(CC) $(CFLAGS) -o main helpers.o main.o 

main.o: main.c helpers.h 
	$(CC) $(CFLAGS) -c main.c

helpers.o: helpers.c helpers.h
	$(CC) $(CFLAGS) -c helpers.c

clean: 
	rm -r main *.o

test:
	python3 testing.py -b 41
