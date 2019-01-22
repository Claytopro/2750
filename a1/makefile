CC = gcc
CPPFLAGS=
CFLAGS = -Wall -pedantic -std=c99 -g
LDFLAGS= -L/home/Schoolprojects/2750/a1/
BINDIR = ./bin/

all: test

test: test.o libcal.so
	$(CC) $(LDFLAGS) $(CFLAGS) -o test test.o -llist -lcal

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

libcal.so: CalendarParser.o liblist.so
	$(CC) -shared -o libcal.so CalendarParser.o

CalendarParser.o: CalendarParser.c CalendarParser.h
	$(CC) $(CFLAGS) -c -fpic CalendarParser.c

liblist.so: LinkedListAPI.o
	$(CC) -shared -o liblist.so LinkedListAPI.o

LinkedListAPI.o: LinkedListAPI.c LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic LinkedListAPI.c

clean:
	rm -rf test *.o *.so

valgrind:
	  valgrind --leak-check=full --track-origins=yes ./test
