/*
By: CLayton Provan

*/

#include "CalendarParser.h"

/** Function to create a Calendar object based on the contents of an iCalendar file.
 *@pre File name cannot be an empty string or NULL.  File name must have the .ics extension.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid calendar has been created, its address was stored in the variable obj, and OK was returned
		or
		An error occurred, the calendar was not created, all temporary memory was freed, obj was set to NULL, and the
		appropriate error code was returned
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param fileName - a string containing the name of the iCalendar file
 *@param a double pointer to a Calendar struct that needs to be allocated
**/
ICalErrorCode createCalendar(char* fileName, Calendar** obj){

  return NULL;
}


/** Function to delete all calendar content and free all the memory.
 *@pre Calendar object exists, is not null, and has not been freed
 *@post Calendar object had been freed
 *@return none
 *@param obj - a pointer to a Calendar struct
**/
void deleteCalendar(Calendar* obj){

}


/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calndar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/
char* printCalendar(const Calendar* obj){

  return NULL;

}


/** Function to "convert" the ICalErrorCode into a humanly redabale string.
 *@return a string contaning a humanly readable representation of the error code by indexing into
          the descr array using rhe error code enum value as an index
 *@param err - an error code
**/
char* printError(ICalErrorCode err){

  return NULL;

}


/** Function to writing a Calendar object into a file in iCalendar format.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a file representing the
        Calendar contents in iCalendar format has been created
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj){
  return NULL;

}


/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj){

  return NULL;

}


// ************* List helper functions - MUST be implemented ***************
void deleteEvent(void* toBeDeleted){

}


int compareEvents(const void* first, const void* second){

  return 0;
}

char* printEvent(void* toBePrinted){


  return NULL;
}

void deleteAlarm(void* toBeDeleted){

}

int compareAlarms(const void* first, const void* second){

  return 0;
}

char* printAlarm(void* toBePrinted){

  return NULL;
}

void deleteProperty(void* toBeDeleted){


}

int compareProperties(const void* first, const void* second){

  return 0;
}

char* printProperty(void* toBePrinted){

  return NULL;
}

void deleteDate(void* toBeDeleted){

}

int compareDates(const void* first, const void* second){

  return 0;
}

char* printDate(void* toBePrinted){

  return NULL;
}
