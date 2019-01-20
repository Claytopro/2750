all: CalendarParser.o test

CalendarParser: CalendarParser.c CalendarParser.h HelperFunction.h
	gcc -Wall -std=c99 -pedantic CalendarParser.c -o CalendarParser

test:test.c
	gcc -Wall -std=c99 -pedantic test.c -o test

valgrind:
	  valgrind --track-origins=yes ./test

clean:
		rm *.o test
