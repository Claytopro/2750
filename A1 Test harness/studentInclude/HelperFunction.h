/*  by:Clayton Provan
*   functioned used to help CalendarParser.c
*/



#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H

#include <ctype.h>
/*
turns a string into property struct
returns pointer to that property.
*/
ICalErrorCode addProperty(char *property, Calendar** obj);

ICalErrorCode addPropertyEvent(char *property, Event** obj);

ICalErrorCode addPropertyAlarm(char *property, Alarm** obj);

//convert a int to a char
char dateBoolToChar(int z);
/*check if DateTime String is valid */
int isValidDateTime(DateTime obj);
//check if input property follows standard
int isValidProperty(Property *obj);

#endif

