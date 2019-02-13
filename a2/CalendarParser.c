/*
By: Clayton Provan 0967777
  Assignement 2

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
    Calendar *tempCal = NULL;
    Event *tempEvent  = NULL;
    Alarm *tempAlarm  = NULL;

    int lineFactor, objectLevel, fileLen, lineLength;
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

    tempCal = calloc(1,sizeof(Calendar));

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
      deleteCalendar(tempCal);
      (*obj) = NULL;
      //printf("ERROR, COUND NOT OPEN FILE \n");
      free(fileExtension);
      free(readLine);
      if(fp != NULL){
        fclose(fp);
      }
      return INV_FILE;
    }
    //fileExtension no longer used
    free(fileExtension);



    /*reads through each character in the file*/
    while(fgets(bufferLine, 80,fp)){
      lineLength = strlen(bufferLine);
      //empty line is foudn and that is bad so we return error, INV_CAL
      if(lineLength == 2){
        if((bufferLine[0] == '\r') && (bufferLine[1] == '\n')){
          (*obj) = NULL;
          free(readLine);
          deleteEvent(tempEvent);
          deleteAlarm(tempAlarm);
          deleteCalendar(tempCal);
          fclose(fp);
          return INV_CAL;
        }
      }
      //check for (CRLF) /r/n line ending
      if(bufferLine[lineLength-1] != '\n' || bufferLine[lineLength-2] != '\r'){
        ////printf("invalid line  :%s:\n",bufferLine);
        //add freeing shit here
        (*obj) = NULL;
        free(readLine);
        deleteEvent(tempEvent);
        deleteAlarm(tempAlarm);
        deleteCalendar(tempCal);
        fclose(fp);
        return INV_FILE;
      }
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
          //this is working :

          ////printf("line is \"%s\"\n",readLine );

          tempStr = strtok(readLine,":;");
          ////printf("%s\n",tempStr);

          if(tempStr != NULL){

            if(strcmp(tempStr,"BEGIN") == 0){
              //used to change what kind of struct is being created
              //cal (1), event(2), Alarma(3)
              tempStr = strtok(NULL,":");

              if(tempStr == NULL){
                tempStr = "";
              }
              switch (objectLevel) {
                //shoudlnt ever really happen unless starting by decalring events or alrams
                //and that would result in
                case 0:
                if(strcmp(tempStr,"VCALENDAR")!=0){
                  ////printf("1inv cal\n");
                  (*obj) = NULL;
                  deleteCalendar(tempCal);

                  free(readLine);
                  fclose(fp);
                  return INV_CAL;
                }
                break;
                case 1:
                if(strcmp(tempStr,"VEVENT")!=0){
                  ////printf("inv event\n" );
                  (*obj) = NULL;
                  deleteCalendar(tempCal);
                  deleteEvent(tempEvent);

                  free(readLine);
                  fclose(fp);
                  return INV_EVENT;
                }else{
                  //valid transformation
                  tempEvent = calloc(1,sizeof(Event));

                  tempEvent->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
                  tempEvent->alarms = initializeList(&printAlarm,&deleteAlarm,&compareAlarms);

                }

                break;
                case 2:
                if((strcmp(tempStr,"VALARM")!=0)){
                  ////printf("INV ALARM\n" );
                  (*obj) = NULL;
                  deleteCalendar(tempCal);
                  deleteEvent(tempEvent);
                  deleteAlarm(tempAlarm);

                  free(readLine);
                  fclose(fp);
                  return INV_ALARM;
                }else{
                  //this is valid
                  tempAlarm = calloc(1,sizeof(Alarm));
                  tempAlarm->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);

                }
                break;
                default:
                //printf("mallformed end\n");
                deleteCalendar(tempCal);
                (*obj)=NULL;
                deleteEvent(tempEvent);
                deleteAlarm(tempAlarm);

                free(readLine);
                fclose(fp);
                return OTHER_ERROR;


              }


              objectLevel++;
            }else if(strcmp(tempStr,"END") == 0){
              tempStr = strtok(NULL,":");
              if(tempStr == NULL){
                //printf("mallformed end\n");
                //set temp to a bad string so it will reach proper error
                tempStr = "";
              }
                switch (objectLevel) {
                  //calendar object should be ending
                  case 1:
                    if(strcmp(tempStr,"VCALENDAR")!=0){
                      //printf("1inv cal\n");
                      (*obj) = NULL;
                      deleteCalendar(tempCal);

                      free(readLine);
                      fclose(fp);
                      return INV_CAL;
                    }
                    if(tempCal->version == 0.00 || strcmp(tempCal->prodID,"")==0){
                      //printf("1inv cal\n");
                      (*obj) = NULL;
                      deleteCalendar(tempCal);

                      free(readLine);
                      fclose(fp);
                    }
                  break;
                  case 2:
                    //if VEVENT HASNT BEEN CLOSED we error;
                    if(strcmp(tempStr,"VEVENT")!=0){
                      //printf("inv event\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);

                      free(readLine);
                      fclose(fp);
                      return INV_EVENT;
                    }

                    if(strcmp(tempEvent->UID,"")==0 || isValidDateTime(tempEvent->creationDateTime) == 1 ||
                  isValidDateTime(tempEvent->startDateTime)== 1){
                      //printf("inv event\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);

                      free(readLine);
                      fclose(fp);
                      return INV_EVENT;
                    }

                    insertBack(tempCal->events,tempEvent);
                  break;
                  case 3:
                    if((strcmp(tempStr,"VALARM")!=0)){
                      //printf("INV ALARM\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);
                      deleteAlarm(tempAlarm);

                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    if(tempAlarm->trigger == NULL || strcmp(tempAlarm->action, "") == 0){
                      //printf("INV ALARM\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);
                      deleteAlarm(tempAlarm);

                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    insertBack(tempEvent->alarms,tempAlarm);
                  break;
                  default:
                  //printf("mallformed end\n");
                  deleteCalendar(tempCal);
                  (*obj)=NULL;
                  deleteEvent(tempEvent);
                  deleteAlarm(tempAlarm);

                  free(readLine);
                  fclose(fp);
                  return OTHER_ERROR;
                }


              objectLevel--;
            }else {
              switch(objectLevel){

                //used when adding to calendar struct
                case 1:

                if(strcmp(tempStr,"VERSION") == 0){
                  //moves to next part of string which should be version number
                  tempStr = strtok(NULL,"");

                  //if version already exists
                  if( tempCal->version != 0.0000){
                    //printf("doop version\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;


                    free(readLine);
                    fclose(fp);
                    return DUP_VER;
                  }

                  if(tempStr == NULL){
                    //printf("INVALUD VERSION\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;


                    free(readLine);
                    fclose(fp);
                    return INV_VER;
                  }
                  //atof will default to 0.0 when a string cant be converted to float
                  tempCal->version = atof(tempStr);

                  if(tempCal->version == 0.0){
                    //printf("INVALUD VERSION\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;

                    free(readLine);
                    fclose(fp);
                    return INV_VER;
                  }
                  break;

                } else if(strcmp(tempStr,"PRODID") == 0){
                  //if PRODID already Exists so we return Error Code
                  if(strcmp(tempCal->prodID,"") != 0){
                    //printf("DOOP prodid\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;

                    free(readLine);
                    fclose(fp);
                    return DUP_PRODID;
                  }

                  tempStr = strtok(NULL,"");
                  //no prodid so we return error
                  if(tempStr == NULL){
                    deleteCalendar(tempCal);
                    (*obj) = NULL;

                    free(readLine);
                    fclose(fp);
                    return INV_PRODID;
                  }

                  strcpy(tempCal->prodID,tempStr);
                  //TODO: remove this print
                  ////printf("proid is %s\n",tempCal->prodID);
                  break;
                }else{

                  //create a property and add it to the propertie list in Caledar object

                  if(addProperty(tempStr,&tempCal) == INV_CAL){
                    //printf("inv cal\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);

                    free(readLine);
                    fclose(fp);
                    return INV_CAL;
                  }
                }

                break;

                //used when adding to event struct
                case 2:
                //create new event, add the shit.
                if(strcmp(tempStr,"UID") == 0){
                  tempStr = strtok(NULL,"");
                  strcpy(tempEvent->UID,tempStr);
                  break;
                }else if(strcmp(tempStr,"DTSTAMP") == 0){
                  tempStr = strtok(NULL,"");
                  if(tempStr == NULL){
                    //printf("inv event\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);

                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }

                  strncpy(tempEvent->creationDateTime.date,tempStr,8);
                  strncpy(tempEvent->creationDateTime.time,tempStr+9,6);

                  if(tempStr[strlen(tempStr)-1] == 'Z'){
                    tempEvent->creationDateTime.UTC = 1;
                  }

                  if(isValidDateTime(tempEvent->creationDateTime)==1){
                    deleteCalendar(tempCal);
                    (*obj) = NULL;

                    free(readLine);
                    //down two levels of abstractions so we delete these

                    deleteEvent(tempEvent);
                    fclose(fp);
                    return INV_DT;
                  }


                }else if(strcmp(tempStr,"DTSTART") == 0){
                  tempStr = strtok(NULL,"");
                  if(tempStr == NULL){
                    //printf("inv event\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);

                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }

                  strncpy(tempEvent->startDateTime.date,tempStr,8);
                  strncpy(tempEvent->startDateTime.time,tempStr+9,6);

                  if(tempStr[strlen(tempStr)-1] == 'Z'){
                    tempEvent->startDateTime.UTC = 1;
                  }

                  if(isValidDateTime(tempEvent->startDateTime)==1){
                    deleteCalendar(tempCal);
                    (*obj) = NULL;

                    free(readLine);
                    //down two levels of abstractions so we delete these
                    deleteEvent(tempEvent);
                    fclose(fp);
                    return INV_DT;
                  }

                }else{
                  if(addPropertyEvent(tempStr,&tempEvent)==INV_EVENT ){
                    //printf("inv event\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);

                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }
                }

                break;
                //used when adding to alarm struct
                case 3:
                  if(strcmp(tempStr,"ACTION") == 0){
                    tempStr = strtok(NULL,"");
                    if(tempStr == NULL){
                      //printf("INV ALARM\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteAlarm(tempAlarm);

                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    strcpy(tempAlarm->action,tempStr);

                  }else if(strcmp(tempStr,"TRIGGER") == 0){
                    tempStr = strtok(NULL,":");
                    if(tempStr == NULL){
                      //printf("INV ALARM\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteAlarm(tempAlarm);

                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    tempAlarm->trigger = malloc(sizeof(char)*strlen(tempStr)+1);
                    strcpy(tempAlarm->trigger,tempStr);
                  }else{
                    //propertie
                    if(addPropertyAlarm(tempStr,&tempAlarm) == INV_ALARM){

                      deleteCalendar(tempCal);
                      (*obj) = NULL;

                      free(readLine);
                      //down two levels of abstractions so we delete these
                      deleteAlarm(tempAlarm);
                      deleteEvent(tempEvent);
                      fclose(fp);
                      return INV_ALARM;
                    }
                  }


                break;
                //error has occurred
                // default:
                // //printf("ERROR ICALENDAR OBJECT BEGAN INCORRECT\n");
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

    /*
    THIS STUFF has to be repeated twice becuase i did not have forthought.
    so we just check the last line like we do before, this stuff wont matter if
    last line is a commnet.

    */
    if(readLine[0] != ';'){
      ////printf("last line is \"%s\"\n",readLine);
      tempStr = strtok(readLine,":;");

      //check if last line is
      if(strcmp(tempStr,"END")==0){
        tempStr = strtok(NULL,"");

        if(tempStr == NULL){
          //printf("mallformed end\n");
          //set temp to a bad string so it will reach proper error
          tempStr = "";
        }

        switch (objectLevel) {
          //calendar object should be ending
          case 1:
            if(strcmp(tempStr,"VCALENDAR")!=0){
              //printf("1inv cal\n");
              (*obj) = NULL;
              deleteCalendar(tempCal);

              free(readLine);
              fclose(fp);
              return INV_CAL;
            }
            if(tempCal->version == 0.00 || strcmp(tempCal->prodID,"")==0){
              //printf("1inv cal\n");
              (*obj) = NULL;
              deleteCalendar(tempCal);

              free(readLine);
              fclose(fp);
              return INV_CAL;
            }
          break;
          case 2:
            if(strcmp(tempStr,"VEVENT")!=0){
              //printf("inv event\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);

              free(readLine);
              fclose(fp);
              return INV_EVENT;
            }
            if(strcmp(tempEvent->UID,"")==0 || isValidDateTime(tempEvent->creationDateTime) == 1 ||
          isValidDateTime(tempEvent->startDateTime)== 1){
              //printf("inv event\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);

              free(readLine);
              fclose(fp);
              return INV_EVENT;
            }
          break;
          case 3:
            if((strcmp(tempStr,"VALARM")!=0)){
              //printf("INV ALARM\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);
              deleteAlarm(tempAlarm);

              free(readLine);
              fclose(fp);
              return INV_ALARM;
            }
            if(tempAlarm->trigger == NULL || strcmp(tempAlarm->action, "") == 0){
              //printf("INV ALARM\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);
              deleteAlarm(tempAlarm);

              free(readLine);
              fclose(fp);
              return INV_ALARM;
            }
          break;
          default:
          //printf("mallformed end\n");
          deleteCalendar(tempCal);
          (*obj)=NULL;
          deleteEvent(tempEvent);
          deleteAlarm(tempAlarm);

          free(readLine);
          fclose(fp);
          return OTHER_ERROR;
        }
        objectLevel--;
      }
    }

    //object level should always be 0 at end of program
    if(objectLevel !=0){
      //printf("inv cal\n");
      (*obj) = NULL;
      deleteCalendar(tempCal);

      free(readLine);
      fclose(fp);
      return INV_CAL;
    }

    //check if tempCalendar is valid
    if(strcmp(tempCal->prodID,"") == 0 ||
    tempCal->version == 0.0 || getFromFront(tempCal->events)==NULL ){
      deleteCalendar(tempCal);
      (*obj) = NULL;
      free(readLine);
      fclose(fp);
      return INV_CAL;
    }

    *obj = tempCal;


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
 *@param obj - a  if(obj == NULL){
      return "Calendar Empty\n";
  } pointer to a Calendar struct
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
  char* toRtrn = malloc(sizeof(char)*100);

  switch(err){
    case OK:
      strcpy(toRtrn,"PROCEDURE WAS OK\n");
    break;
    case INV_FILE:
      strcpy(toRtrn,"INVALID FILE\n");
    break;
    case INV_CAL:
      strcpy(toRtrn,"INVALID CALENDAR\n");
    break;
    case INV_VER:
      strcpy(toRtrn,"INVALID VERSION\n");
    break;
    case DUP_VER:
      strcpy(toRtrn, "DUPLICATE VERSION APPEARED\n");
    break;
    case INV_PRODID:
      strcpy(toRtrn,"INVALID PRODUCT ID\n");
    break;
    case DUP_PRODID:
      strcpy(toRtrn,"DUPULICATE PRODUCT ID\n");
    break;
    case INV_EVENT:
      strcpy(toRtrn,"INVALID EVENT\n");
    break;
    case INV_DT:
      strcpy(toRtrn,"INVALID DATETIME\n");
    break;
    case INV_ALARM:
      strcpy(toRtrn, "INVALID ALARM\n");
    break;
    case WRITE_ERROR:
      strcpy(toRtrn, "WRITE ERROR\n");
    break;
    case OTHER_ERROR:
      strcpy(toRtrn,"OTHER ERROR OCCURED\n");
    break;
    default:
      strcpy(toRtrn,"UNKOWN ERROR OCCURED\n");
  }

  return toRtrn;

}


/** Function to writing a Calendar object into a file in iCalendar format.
 *@pre Calendar object exists, is not null, and is valid
 *@post Calendar has not been modified in any way, and a file representing the
        Calendar contents in iCalendar format has been created
 *@return the error code indicating success or the error encountered when parsing the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode writeCalendar(char* fileName, const Calendar* obj){
  FILE *fp;
  char *toRtrn;
  char *temp;
  void* elem;
  ListIterator iter;

  fp = fopen(fileName,"w+");
  if(fp == NULL){
    return INV_FILE;
  }

  //  initilizes values to 0, unlike malloc.
  toRtrn = calloc(100,sizeof(char));
  temp = calloc(100,sizeof(char));

  strcpy(toRtrn,"BEGIN:VCALENDAR\r\n");
  sprintf(temp,"VERSION:%.2f\r\n",obj->version);
  //+14 to not get valgrind errors, unsure why must ask TA, its because sizeof() returns 8
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+2);
  strcat(toRtrn,temp);

  sprintf(temp,"PRODID:%s\r\n",obj->prodID);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+2);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack
  iter = createIterator(obj->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

  while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+2);
    strcat(toRtrn,temp);
    free(temp);
  }

  iter = createIterator(obj->events);
  while ((elem = nextElement(&iter)) != NULL){
    Event* tempEvent = (Event*)elem;
    temp = printEvent(tempEvent);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+2);
    strcat(toRtrn,temp);
    free(temp);
  }
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +16));
  strcat(toRtrn,"END:VCALENDAR\r\n");

  fwrite(toRtrn,strlen(toRtrn)*sizeof(char),1,fp);
  free(toRtrn);
  fclose(fp);
  return OK;
}

/** Function to validating an existing a Calendar object
 *@pre Calendar object exists and is not null
 *@post Calendar has not been modified in any way
 *@return the error code indicating success or the error encountered when validating the calendar
 *@param obj - a pointer to a Calendar struct
 **/
ICalErrorCode validateCalendar(const Calendar* obj){
  ListIterator iter,iterTwo,iterThree;
  void* elem;
  void* elemTwo;
  void* elemThree;
  //ensure Calendar has UID, creation/start date
  if(strcmp(obj->prodID,"")== 0) return INV_CAL;
  if(obj->version <= 0) return INV_CAL;
  if(getFromFront(obj->events) == NULL) return INV_CAL;

  iter = createIterator(obj->events);
  //check if all events are valid
  while ((elem = nextElement(&iter)) != NULL){
    Event* tempEvent = (Event*)elem;
    if(tempEvent == NULL) return INV_CAL;
    if(strcmp(tempEvent->UID, "")==0) return INV_CAL;
    //check if dates are valid
    if(isValidDateTime(tempEvent->creationDateTime) ==1)return INV_CAL;
    if(isValidDateTime(tempEvent->startDateTime)==1) return INV_CAL;

    iterTwo = createIterator(tempEvent->alarms);
    //check if all alarms are valid within an event
    while ((elemTwo = nextElement(&iterTwo))!=NULL){
      Alarm* tempAlarm = (Alarm*)elemTwo;
      if(tempAlarm == NULL) return INV_CAL;
      if(strcmp(tempAlarm->action,"")==0)return INV_CAL;
      if(tempAlarm->trigger == NULL) return INV_CAL;

      //check all properties are valid within alarm
      iterThree = createIterator(tempAlarm->properties);
      while((elemThree = nextElement(&iterThree)) != NULL){
        Property* tempProp = (Property*)elemThree;
        if(isValidProperty(tempProp) == 0) return INV_CAL;

      }
    }

    //check Event properties for validity
    iterThree = createIterator(tempEvent->properties);
    while((elemThree = nextElement(&iterThree)) != NULL){
      Property* tempProp = (Property*)elemThree;
      if(isValidProperty(tempProp) == 0) return INV_CAL;
    }
  }

  iterThree = createIterator(obj->properties);
  while((elemThree = nextElement(&iterThree)) != NULL){
    Property* tempProp = (Property*)elemThree;
    if(isValidProperty(tempProp) == 0) return INV_CAL;

  }


  return OK;
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
  Event* tmpEvent1;
  Event* tmpEvent2;

  if (first == NULL || second == NULL){
    return 0;
  }

  tmpEvent2 = (Event*)second;
  tmpEvent1 = (Event*)first;

  if(strcmp(tmpEvent1->UID,tmpEvent2->UID) == 0){
    return 1;
  }


  return 0;
}

char* printEvent(void* toBePrinted){
  char *toRtrn = NULL;
  char *temp   = NULL;
  char *tempDT = NULL;
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

  tempDT = printDate(&(tempEvent->creationDateTime));
  sprintf(temp,"DTSTAMP:%s\n",tempDT);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);
  free(tempDT);

  tempDT = printDate(&(tempEvent->startDateTime));
  sprintf(temp,"DTSTART:%s\n",tempDT);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);
  free(tempDT);

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
  if(tempAlarm->trigger != NULL){
    free(tempAlarm->trigger);
  }


    freeList(tempAlarm->properties);

  free(tempAlarm);
}

int compareAlarms(const void* first, const void* second){
  Alarm* tmpAlarm1;
  Alarm* tmpAlarm2;

  if (first == NULL || second == NULL){
		return 0;
	}

  tmpAlarm2 = (Alarm*)second;
  tmpAlarm1 = (Alarm*)first;

  if(strcmp(tmpAlarm1->action,tmpAlarm2->action) == 0 &&strcmp(tmpAlarm1->trigger,tmpAlarm2->trigger) == 0 ){
    return 1;
  }

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
  //printf("deleting %s\n",printProperty(//tempProp) );
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
  sprintf(toRtrn,"%s;%s",tempProp->propName,tempProp->propDescr);


  return toRtrn;
}

void deleteDate(void* toBeDeleted){
  DateTime *tempDate;

  if (toBeDeleted == NULL){
    return;
  }
  tempDate = (DateTime*)toBeDeleted;

  free(tempDate);
}

int compareDates(const void* first, const void* second){
  DateTime* tmpDate1;
  DateTime* tmpDate2;

  if (first == NULL || second == NULL){
		return 0;
	}

  tmpDate2 = (DateTime*)second;
  tmpDate1 = (DateTime*)first;

  if(strcmp(tmpDate1->date,tmpDate2->date)==0 && strcmp(tmpDate1->time,tmpDate2->time)==0){
    return 1;
  }


  return 0;
}

char* printDate(void* toBePrinted){
  DateTime *tempDate;
  char *toRtrn;

  if(toBePrinted == NULL){
    return NULL;
  }

  tempDate = (DateTime*)toBePrinted;


  toRtrn = calloc(1,sizeof(DateTime)+1);
  sprintf(toRtrn,"%sT%s%c",tempDate->date,tempDate->time,dateBoolToChar(tempDate->UTC));

  return toRtrn;
}


ICalErrorCode addProperty(char *property, Calendar** obj){
    Property *newProperty = malloc(sizeof(Property));
    //char *tmp;
    strcpy(newProperty->propName,property);
    property = strtok(NULL,"");
    //realloc to account for size of description becuase of dynamic array in Property struct
    newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
    strcpy(newProperty->propDescr,property);

    if(property == NULL){
      deleteProperty(newProperty);
      return INV_CAL;
    }
    // tmp = printProperty(newProperty);
    // ////printf("property to create: %s\n",tmp);
    // free(tmp);

    insertFront((*obj)->properties,newProperty);

    return OK;
}

ICalErrorCode addPropertyEvent(char *property, Event** obj){
  Property *newProperty = malloc(sizeof(Property));
  //  char *tmp;
  strcpy(newProperty->propName,property);

  //will turn the rest of the tokenized thing into the description of property
  property = strtok(NULL,"");
  //realloc to account for size of description becuase of dynamic array in Property struct
  newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
  strcpy(newProperty->propDescr,property);


  if(property == NULL){
    deleteProperty(newProperty);
    return INV_EVENT;
  }
  // tmp = printProperty(newProperty);
  // ////printf("Event property to create:%s!\n",tmp);
  // free(tmp);

  insertFront((*obj)->properties,newProperty);

  return OK;
}

ICalErrorCode addPropertyAlarm(char *property, Alarm** obj){
  Property *newProperty = malloc(sizeof(Property));
  strcpy(newProperty->propName,property);
  property = strtok(NULL,"");

  //indicates malformed
  if(property == NULL){
    deleteProperty(newProperty);
    return INV_ALARM;
  }

  //realloc to account for size of description becuase of dynamic array in Property struct
  newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
  strcpy(newProperty->propDescr,property);

  // char *tmp;
  // tmp = printProperty(newProperty);
  // //printf("Alarm property to create: %s\n",tmp);
  // free(tmp);

  insertFront((*obj)->properties,newProperty);

  return OK;
}


char dateBoolToChar(int z){
  if(z == 1){
    return 'Z';
  }

  return '\0';
}
// return 1 if invalid
int isValidDateTime(DateTime obj){
    int i;
    //checks date for any numbers, should be valid if its just numbers
    for(i=0;i<8;i++){
      if(isdigit(obj.date[i]) == 0){
        return 1;
      }
    }
    //checks time for only containing numbers
    for(i=0;i<6;i++){
      if(isdigit(obj.time[i]) == 0){
        return 1;
      }
    }

    if(obj.UTC != 1 && obj.UTC != 0) return 1;

    return 0;
}

//checks if input propertie is valud
int isValidProperty(Property *obj){
  if(obj == NULL) return 0;
  
  if(strcmp(obj->propName,"") == 0) return 0;

  if(strcmp(obj->propDescr,"")==0) return 0;

  return 1;
}
