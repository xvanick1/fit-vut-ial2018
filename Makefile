CC=gcc
CFLAGS=-std=gnu99 -Wall -pedantic

all: helpers.o main.o list.o
	$(CC) $(CFLAGS) -o main helpers.o main.o list.o

main.o: main.c helpers.h
	$(CC) $(CFLAGS) -c main.c

helpers.o: helpers.c helpers.h
	$(CC) $(CFLAGS) -c helpers.c

list.o: list.c helpers.h
	$(CC) $(CFLAGS) -c list.c

clean: 
	rm -r main *.o

test:
	python3 testing.py -b 40
