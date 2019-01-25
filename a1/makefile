CC = gcc
CPPFLAGS=
CFLAGS = -Wall -std=c11 -g
LDFLAGS= -L./bin/
BINDIR = ./bin/
SRCDIR = ./src/

all: test

test: test.o libcal.so
	$(CC) $(CFLAGS) -o test test.o -llist -lcal $(LDFLAGS)

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

libcal.so: CalendarParser.o liblist.so
	$(CC) -shared -o $(BINDIR)libcal.so CalendarParser.o

CalendarParser.o: CalendarParser.c CalendarParser.h
	$(CC) $(CFLAGS) -c -fpic CalendarParser.c

LinkedListAPI.o: LinkedListAPI.c LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic LinkedListAPI.c

liblist.so: LinkedListAPI.o
	$(CC) -shared -o $(BINDIR)liblist.so LinkedListAPI.o

clean:
	rm -rf test *.o $(BINDIR)*.so

valgrind:
	  valgrind --leak-check=full --track-origins=yes ./test
