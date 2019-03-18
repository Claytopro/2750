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
//check if input property is valid Calendar property
int isValidProperty(Property *obj);
//check if valid Event Property;
int isValidEventProperty(Property *obj,Event *tempEvent);

int isValidAlarmProperty(Property *obj,Alarm *tempAlarm);
//sets to upper case
void strUpper(char string[]);

ListIterator createConstIterator(const List* list);

Calendar* nodeCreateCal(char* fileName);

void nodeWriteCAl(char* fileName, Calendar* obj);

char* nodeEventListJSON(Calendar* obj);

char* alarmToJSON(Alarm *prop);


#endif
