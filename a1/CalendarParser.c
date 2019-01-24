/*
By: CLayton Provan

*/

#include "CalendarParser.h"
#include "HelperFunction.h"
#include "LinkedListAPI.h"
#include "LinkedListAPI.c"

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
    Calendar *tempCal = NULL;
    Property *tempProp = NULL;
    Event *tempEvent;
    Alarm *tempAlarm;

    int lineFactor, objectLevel, fileLen;
    //used to hold full line. must be dynamically
    // allocated to allow for mutipel folded lines
    char *readLine = NULL;
    //used to hold the line of information
    //holds 75 bytes, 1 null poiner and potentially
    //3 extra bytes to indicate line break"/r/n "
    char bufferLine[80];
    //temp string for processing data
    char *tempStr = NULL;
    char *fileExtension = NULL;

    //used to measure ammount of memory need to reallocate for folded lines
    lineFactor =1;
    objectLevel =0;


    // TODO : check for malloc erros
    tempProp = malloc(sizeof(Property));
    tempCal = malloc(sizeof(Calendar));

    //OLD:readLine = malloc(sizeof(char)*(80*lineFactor));
    //use calloc to fix valgring conditional jump errors
    readLine = calloc(80,sizeof(char));
    //readLine = NULL;
  //  bufferLine = malloc(sizeof(char)*(80*lineFactor));

    //intilize properties linkedlist in calendar
    tempCal->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
    tempCal->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
    /*open file for reading*/
    fp = fopen(fileName, "r+");

    //test for extension matching .ics and if file exists
    fileLen = strlen(fileName);
    fileExtension = malloc(sizeof(char)*fileLen +2);
    strcpy(fileExtension, &fileName[fileLen-4]);

    if( fp == NULL ||  strcmp(".ics",fileExtension) != 0){
      /* TODO:Change TO ERROR CODE */
      printf("ERROR, COUND NOT OPEN FILE \n");
      return INV_FILE;
    }



    /*reads through each character in the file*/
    while(fgets(bufferLine, 80,fp)){
      //remove newline, should be fine for new line at beginnign becayse fgets will read until then
      bufferLine[strcspn(bufferLine, "\r\n")] = 0;

      //this line is a comment and we can ignore it.
      if(bufferLine[0] != ';'){
        //test for folded line, else line is valid and can be parsed
        if(bufferLine[0] == ' ' || bufferLine[0] == '\t'){
          lineFactor++;
          //realloc enough memory to concatonate strings together.
          readLine = realloc(readLine,sizeof(char)*(80*lineFactor));

          //gets rid of space or half tab
          memmove(bufferLine, bufferLine+1, strlen(bufferLine));

          strcat(readLine,bufferLine);
          // now you may have you full line, must check if next is folded as well
        }else{
          //now readLine is the full line! greeat
          //this is working : printf("line is \"%s\"\n",readLine );

          tempStr = strtok(readLine,":");

          if(tempStr != NULL){

            if(strcmp(tempStr,"BEGIN") == 0){
              //used to change what kind of struct is being created
              //cal (1), event(2), Alarma(3)
              tempStr = strtok(NULL,":");

              if(strcmp(tempStr,"VCALENDAR")==0 && objectLevel ==0){
                //this is valid
              }else if(strcmp(tempStr,"VEVENT")==0 && objectLevel ==1){
                //valid transformation

                //calloc should initilize datetime so i dont get valgrind erros
                tempEvent = calloc(1,sizeof(Event));

                tempEvent->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
                tempEvent->alarms = initializeList(&printAlarm,&deleteAlarm,&compareAlarms);

              } else if(strcmp(tempStr,"VALARM")==0 && objectLevel ==2){
                //this is valid
                tempAlarm = malloc(sizeof(Alarm));
                tempAlarm->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);

              }else{
                //no valid transformation, print error
              }


              objectLevel++;
            }else if(strcmp(tempStr,"END") == 0){
              tempStr = strtok(NULL,":");

              if(strcmp(tempStr,"VCALENDAR")==0 && objectLevel ==1){
                //this is valid and program should be ending
              }else if(strcmp(tempStr,"VEVENT")==0 && objectLevel ==2){
                //valid transformation and should add Event to Calendar event linkedlist
                /*TODO: add events to list*/

                insertFront(tempCal->events,tempEvent);

              } else if(strcmp(tempStr,"VALARM")==0 && objectLevel ==3){
                //this is valid and should be adding alarm to current event.
                insertFront(tempEvent->alarms,tempAlarm);

              }else{
                //no valid transformation, print error
              }

              objectLevel--;
            }else {
              switch(objectLevel){

                //used when adding to calendar struct
                case 1:

                if(strcmp(tempStr,"VERSION") == 0){
                  //moves to next part of string which should be version number
                  tempStr = strtok(NULL,":");
                  tempCal->version = atof(tempStr);
                  if(tempCal->version == 0.0){
                    //atof will default to 0.0 when a string cant be converted to float
                    printf("INVALUD VERSION\n");
                    return INV_VER;
                  }
                  break;

                } else if(strcmp(tempStr,"PRODID") == 0){
                  tempStr = strtok(NULL,":");
                  strcpy(tempCal->prodID,tempStr);
                  //TODO: remove this print
                  //printf("proid is %s\n",tempCal->prodID);
                  break;
                }else{

                  //create a property and add it to the propertie list in Caledar object
                  addProperty(tempStr,&tempCal);
                }

                break;

                //used when adding to event struct
                case 2:
                //create new event, add the shit.
                if(strcmp(tempStr,"UID") == 0){
                  tempStr = strtok(NULL,":");
                  strcpy(tempEvent->UID,tempStr);
                  break;
                }else if(strcmp(tempStr,"DTSTAMP") == 0){
                  tempStr = strtok(NULL,":");

                  strncpy(tempEvent->creationDateTime.date,tempStr,8);
                  strncpy(tempEvent->creationDateTime.time,tempStr+9,6);

                  if(tempStr[strlen(tempStr)-1] == 'Z'){
                    tempEvent->creationDateTime.UTC = 1;
                  }

                  //printf("dates and such:%s:%s:%d:\n",tempDateTime->date,tempDateTime->time,tempDateTime->UTC);
                  //not sure if this is right becuase its not a pointer in struct
                  //but gotta do it this way for dynamic allocation so it might be
                  //some shit like &tempdate or (*tmepdate)... idk

                }else if(strcmp(tempStr,"DTSTART") == 0){
                  tempStr = strtok(NULL,":");

                  strncpy(tempEvent->startDateTime.date,tempStr,8);
                  strncpy(tempEvent->startDateTime.time,tempStr+9,6);

                  if(tempStr[strlen(tempStr)-1] == 'Z'){
                    tempEvent->startDateTime.UTC = 1;
                  }
                }else{
                  addPropertyEvent(tempStr,&tempEvent);
                }

                break;
                //used when adding to alarm struct
                case 3:
                  if(strcmp(tempStr,"ACTION") == 0){
                    tempStr = strtok(NULL,":");
                    strcpy(tempAlarm->action,tempStr);

                  }else if(strcmp(tempStr,"TRIGGER") == 0){
                    tempStr = strtok(NULL,":");
                    tempAlarm->trigger = malloc(sizeof(char)*strlen(tempStr)+1);
                    strcpy(tempAlarm->trigger,tempStr);
                  }else{
                    //propertie
                    addPropertyAlarm(tempStr,&tempAlarm);
                  }


                break;
                //error has occurred
                default:
                printf("ERROR ICALENDAR OBJECT BEGAN INCORRECT\n");
              }

            }



          }

          //resets readLine and copies next line to it. as well resets factor for reallocing readLine
          //makes it so last line is not processed in loop and is delt with afterwards
          memset(readLine,0,strlen(readLine));
          strcat(readLine,bufferLine);
          lineFactor =1;
        }
      }


    }

    //do stuff with final line here
    //printf("line is \"%s\"\n",readLine);


    *obj = tempCal;

    //TODO move this(free tempProp) shit into delete calendar function once we get linked list codes
    free(tempProp);
    free(fileExtension);
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

// causes error. dont know why
//  free(obj->prodID);

//change to dlete list
  if(obj->properties != NULL){
    freeList(obj->properties);
  }
  if(obj->events != NULL){
    freeList(obj->events);
  }

  free(obj);

}


/** Function to create a string representation of a Calendar object.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a string representing the Calndar contents has been created
 *@return a string contaning a humanly readable representation of a Calendar object
 *@param obj - a pointer to a Calendar struct
**/
char* printCalendar(const Calendar* obj){
  char *toRtrn;
  char *temp;
  void* elem;
  ListIterator iter;


//  initilizes values to 0, unlike malloc.
  toRtrn = calloc(100,sizeof(char));
  temp = calloc(100,sizeof(char));

  strcpy(toRtrn,"\nBEGIN:VCALENDAR\n");
  sprintf(temp,"VERSION:%.2f\n",obj->version);
  //+14 to not get valgrind errors, unsure why must ask TA, its because sizeof() returns 8
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"PRODID:%s\n",obj->prodID);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack
	iter = createIterator(obj->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

	while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}

  iter = createIterator(obj->events);
  while ((elem = nextElement(&iter)) != NULL){
    Event* tempEvent = (Event*)elem;
    temp = printEvent(tempEvent);
    strcat(temp,"\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +15));
  strcat(toRtrn,"END:VCALENDAR\n");
  return toRtrn;
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
  Event *tempEvent;

  if (toBeDeleted == NULL){
		return;
	}
  tempEvent = (Event*)toBeDeleted;

  if(tempEvent->properties != NULL){
    freeList(tempEvent->properties);
  }
  if(tempEvent->alarms != NULL){
    freeList(tempEvent->alarms);
  }
  //printf("deleting %s\n",printProperty(tempProp) );
  free(tempEvent);
}


int compareEvents(const void* first, const void* second){

  return 0;
}

char* printEvent(void* toBePrinted){
  char *toRtrn = NULL;
  char *temp   = NULL;
  void* elem;
  Event* tempEvent = NULL;
  ListIterator iter;

  if(toBePrinted == NULL){
    return NULL;
  }

  tempEvent = (Event*)toBePrinted;

//  initilizes values to 0, unlike malloc.
  toRtrn = calloc(100,sizeof(char));
  temp = calloc(1000,sizeof(char));

  strcpy(toRtrn,"BEGIN:VEVENT\n");
  sprintf(temp,"UID:%s\n",tempEvent->UID);

  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"DTSTAMP:%sT%s%d\n",tempEvent->creationDateTime.date,tempEvent->creationDateTime.time,tempEvent->creationDateTime.UTC);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"DTSTART:%sT%s%d\n",tempEvent->creationDateTime.date,tempEvent->creationDateTime.time,tempEvent->creationDateTime.UTC);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack
	iter = createIterator(tempEvent->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

	while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}

  iter = createIterator(tempEvent->alarms);
  while ((elem = nextElement(&iter)) != NULL){
    Alarm* tempAlarm = (Alarm*)elem;
    temp = printAlarm(tempAlarm);
    strcat(temp,"\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}


  toRtrn = realloc(toRtrn, (strlen(toRtrn) +12));
  strcat(toRtrn,"END:EVENT");

  return toRtrn;

}

void deleteAlarm(void* toBeDeleted){
  Alarm *tempAlarm;

  if (toBeDeleted == NULL){
		return;
	}
  tempAlarm = (Alarm*)toBeDeleted;
  free(tempAlarm->trigger);

  if(tempAlarm->properties != NULL){
    freeList(tempAlarm->properties);
  }
  free(tempAlarm);
}

int compareAlarms(const void* first, const void* second){

  return 0;
}

char* printAlarm(void* toBePrinted){
  char *toRtrn = NULL;
  char *temp   = NULL;
  void* elem;
  Alarm* tempAlarm = NULL;
  ListIterator iter;

  if(toBePrinted == NULL){
    return NULL;
  }

  tempAlarm = (Alarm*)toBePrinted;

//  initilizes values to 0, unlike malloc.
  toRtrn = calloc(1000,sizeof(char));
  temp = calloc(201,sizeof(char));
  strcpy(toRtrn,"BEGIN:VALARM\n");
  sprintf(temp,"ACTION:%s\n",tempAlarm->action);

  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"TRIGGER:%s\n",tempAlarm->trigger);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack
  if(tempAlarm->properties == NULL){
    printf("tester\n");
  }

	iter = createIterator(tempAlarm->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

	while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +12));
  strcat(toRtrn,"END:VALARM");

  return toRtrn;
}


void deleteProperty(void* toBeDeleted){
  Property *tempProp;

  if (toBeDeleted == NULL){
		return;
	}
  tempProp = (Property*)toBeDeleted;
  //printf("deleting %s\n",printProperty(tempProp) );
  free(tempProp);
}

int compareProperties(const void* first, const void* second){
  Property* tmpProp1;
  Property* tmpProp2;

  if (first == NULL || second == NULL){
		return 0;
	}

  tmpProp2 = (Property*)second;
  tmpProp1 = (Property*)first;

  if(strcmp(tmpProp1->propDescr,tmpProp2->propDescr) == 0 && strcmp(tmpProp1->propName,tmpProp2->propName) == 0){
    return 1;
  }

  return 0;
}

char* printProperty(void* toBePrinted){
  Property *tempProp;
  char *toRtrn;

  if(toBePrinted == NULL){
    return NULL;
  }

  tempProp = (Property*)toBePrinted;

  toRtrn = calloc(201+ strlen(tempProp->propDescr),sizeof(char));
  sprintf(toRtrn,"%s:%s",tempProp->propName,tempProp->propDescr);


  return toRtrn;
}

void deleteDate(void* toBeDeleted){

}

int compareDates(const void* first, const void* second){

  return 0;
}

char* printDate(void* toBePrinted){

  return NULL;
}


ICalErrorCode addProperty(char *property, Calendar** obj){
    Property *newProperty = malloc(sizeof(Property));
    char *tmp;
    strcpy(newProperty->propName,property);
    property = strtok(NULL,"");
    //realloc to account for size of description becuase of dynamic array in Property struct
    newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
    strcpy(newProperty->propDescr,property);

    tmp = printProperty(newProperty);
    //printf("property to create: %s\n",tmp);
    free(tmp);

    insertFront((*obj)->properties,newProperty);

    return OK;
}

ICalErrorCode addPropertyEvent(char *property, Event** obj){
  Property *newProperty = malloc(sizeof(Property));
  char *tmp;
  strcpy(newProperty->propName,property);

  //will turn the rest of the tokenized thing into the description of property
  property = strtok(NULL,"");
  //realloc to account for size of description becuase of dynamic array in Property struct
  newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
  strcpy(newProperty->propDescr,property);

  tmp = printProperty(newProperty);
  //printf("Event property to create:%s!\n",tmp);
  free(tmp);

  insertFront((*obj)->properties,newProperty);

  return OK;
}

ICalErrorCode addPropertyAlarm(char *property, Alarm** obj){
  Property *newProperty = malloc(sizeof(Property));
  char *tmp;
  strcpy(newProperty->propName,property);
  property = strtok(NULL,"");
  //realloc to account for size of description becuase of dynamic array in Property struct
  newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
  strcpy(newProperty->propDescr,property);

  tmp = printProperty(newProperty);
  //printf("Alarm property to create: %s\n",tmp);
  free(tmp);

  insertFront((*obj)->properties,newProperty);

  return OK;
}
