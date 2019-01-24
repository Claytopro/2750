/*  by:Clayton Provan
*   functioned used to help CalendarParser.c
*/



#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H

/*
turns a string into property struct
returns pointer to that property.
*/
ICalErrorCode addProperty(char *property, Calendar** obj);

ICalErrorCode addPropertyEvent(char *property, Event** obj);

ICalErrorCode addPropertyAlarm(char *property, Alarm** obj);

#endif
