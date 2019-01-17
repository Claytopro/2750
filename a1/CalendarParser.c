/*
By: CLayton Provan

*/

#include "CalendarParser.h"
#include "HelperFunction.h"

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
    FILE *fp;
    Calendar *tempCal;
    int lineFactor;
    //used to hold full line. must be dynamically
    // allocated to allow for mutipel folded lines
    char *readLine;
    //used to hold the line of information
    //holds 75 bytes, 1 null poiner and potentially
    //3 extra bytes to indicate line break"/r/n "
    char *bufferLine;

    //used to measure ammount of memory need to reallocate for folded lines
    lineFactor =1;

    tempCal = malloc(sizeof(Calendar));
    readLine = malloc(sizeof(char)*(80*lineFactor));
    bufferLine = malloc(sizeof(char)*(80*lineFactor));

    /*open file for reading*/
    fp = fopen(fileName, "r+");
    if( fp == NULL){
      /* TODO:Change TO ERROR CODE */
      printf("ERROR, COUND NOT OPEN FILE \n");
      return INV_FILE;
    }



    /*reads through each character in the file*/
    while(fgets(bufferLine, 80,fp)){
      //remove newline, should be fine for new line at beginnign becayse fgets will read until then
      bufferLine[strcspn(bufferLine, "\r\n")] = 0;

      //test for folded line, else line is valid and can be parsed
      if(bufferLine[0] == ' ' || bufferLine[0] == '\t'){

        lineFactor++;
        readLine = realloc(readLine,sizeof(char)*(80*lineFactor));
        //gets rid of space or half tab
        bufferLine++;

        strcat(readLine,bufferLine);
        // now you may have you full line, must check if next is folded as well
      }else{
        //now readLine is the full line! greeat
        printf("line is \"%s\"\n",readLine );

        //do stuff

        //resets readLine and copies next line to it. as well resets factor for reallocing readLine
        //makes it so last line is not processed in loop and is delt with afterwards
        memset(readLine,0,strlen(readLine));
        strcat(readLine,bufferLine);
        lineFactor =1;
      }


    }

    //do stuff with final line here
    printf("line is \"%s\"\n",readLine);


    *obj = tempCal;
    //dont forget to use strcpy

    free(readLine);
    fclose(fp);

  return OK;
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
  return INV_FILE;

}


/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj){

  return INV_FILE;

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
