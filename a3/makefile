CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS = -L.
BINDIR = ./bin/

all: libcal.so liblist.so
	$(CC) $(CFLAGS) -o test test.c -llist -lcal $(LDFLAGS)

libcal.so: CalendarParser.o
	$(CC) -shared -o libcal.so CalendarParser.o

CalendarParser.o: CalendarParser.c CalendarParser.h
	$(CC) $(CLAGS) -c -fpic CalendarParser.c

liblist.so: LinkedListAPI.o
	$(CC) $(CFLAGS) -shared -o liblist.so LinkedListAPI.o

LinkedListAPI.o: LinkedListAPI.c LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic LinkedListAPI.c

clean:
	rm -rf test *.o *.so

exPATH:
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."

valgrind:
	valgrind --leak-check=full --track-origins=yes ./test
