CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS = -L~/bin
BIN = ./bin/
SRC = ./src/
INC = ./include/

all: parser 

parser: CalendarParser.o LinkedListAPI.o
	$(CC) -shared -o $(BIN)libcal.so $(BIN)CalendarParser.o $(BIN)LinkedListAPI.o

CalendarParser.o: $(SRC)CalendarParser.c $(INC)CalendarParser.h
	$(CC) $(CFLAGS) -c -fpic $(SRC)CalendarParser.c -Iinclude -o $(BIN)CalendarParser.o

LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic $(SRC)LinkedListAPI.c -Iinclude -o $(BIN)LinkedListAPI.o

clean:
	rm -rf test $(BIN)*.o $(BIN)*.so

exPATH:
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:."
