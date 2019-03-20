/*
By: CLayton Provan
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
    char bufferLine[20000];
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
    readLine = calloc(20000,sizeof(char));

    //readLine = NULL;
  //  bufferLine = malloc(sizeof(char)*(80*lineFactor));

    //intilize properties linkedlist in calendar
    tempCal->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
    tempCal->events = initializeList(&printEvent,&deleteEvent,&compareEvents);
    /*open file for reading*/
    if(fileName == NULL){
      //deleteCalendar(tempCal);
      (*obj) = NULL;
      free(fileExtension);
      free(readLine);
      return INV_FILE;
    }

    fp = fopen(fileName, "r+");

    //test for extension matching .ics and if file exists
    fileLen = strlen(fileName);
    fileExtension = malloc(sizeof(char)*fileLen +2);
    strcpy(fileExtension, &fileName[fileLen-4]);

    if( fp == NULL ||  strcmp(".ics",fileExtension) != 0){
      /* TODO:Change TO ERROR CODE */
      deleteCalendar(tempCal);
      (*obj) = NULL;
      //printf("ERROR, COUND NOT OPEN FILE \r\n");
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
    while(fgets(bufferLine, 20000,fp)){
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
        ////printf("invalid line  :%s:\r\n",bufferLine);
        //add freeing shit here
        (*obj) = NULL;
        free(readLine);
        deleteEvent(tempEvent);
        deleteAlarm(tempAlarm);
        deleteCalendar(tempCal);
        fclose(fp);
        printf("FAILED EHRERE %s\n",bufferLine );
        return INV_FILE;
      }
      //remove newline, should be fine for new line at beginnign becayse fgets will read until then
      bufferLine[strcspn(bufferLine, "\r\r\n")] = 0;

      //this line is a comment and we can ignore it.
      if(bufferLine[0] != ';'){
        //test for folded line, else line is valid and can be parsed
        if(bufferLine[0] == ' ' || bufferLine[0] == '\t'){

          lineFactor++;
          //realloc enough memory to concatonate strings together.
          readLine = realloc(readLine,sizeof(char)*(20000*lineFactor));

          //gets rid of space or half tab
          memmove(bufferLine, bufferLine+1, strlen(bufferLine));

          strcat(readLine,bufferLine);
          // now you may have you full line, must check if next is folded as well
        }else{
          //now readLine is the full line! greeat
          //this is working :

          ////printf("line is \"%s\"\r\n",readLine );

          if(readLine[0] == ':'){
            tempStr = "";
          }else{

            tempStr = strtok(readLine,":;");
            //printf("str = :%s:\r\n",tempStr);
            if(tempStr == NULL){
              tempStr = "";
            }
          }



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
                  ////printf("1inv cal\r\n");
                  (*obj) = NULL;
                  deleteCalendar(tempCal);

                  free(readLine);
                  fclose(fp);
                  return INV_CAL;
                }
                break;
                case 1:
                if(strcmp(tempStr,"VEVENT")!=0){
                  ////printf("inv event\r\n" );
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
                  ////printf("INV ALARM\r\n" );
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
                //printf("mallformed end\r\n");
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
                //printf("mallformed end\r\n");
                //set temp to a bad string so it will reach proper error
                tempStr = "";
              }
                switch (objectLevel) {
                  //calendar object should be ending
                  case 1:
                    if(strcmp(tempStr,"VCALENDAR")!=0){
                      //printf("1inv cal\r\n");
                      (*obj) = NULL;
                      deleteCalendar(tempCal);

                      free(readLine);
                      fclose(fp);
                      return INV_CAL;
                    }
                    if(tempCal->version == 0.00 || strcmp(tempCal->prodID,"")==0){
                      //printf("1inv cal\r\n");
                      (*obj) = NULL;
                      deleteCalendar(tempCal);

                      free(readLine);
                      fclose(fp);
                    }
                  break;
                  case 2:
                    //if VEVENT HASNT BEEN CLOSED we error;
                    if(strcmp(tempStr,"VEVENT")!=0){
                      //printf("inv event\r\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);

                      free(readLine);
                      fclose(fp);
                      return INV_EVENT;
                    }

                    if(strcmp(tempEvent->UID,"")==0 || isValidDateTime(tempEvent->creationDateTime) == 1 ||
                  isValidDateTime(tempEvent->startDateTime)== 1){
                      //printf("inv event\r\n" );
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
                      //printf("INV ALARM\r\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteEvent(tempEvent);
                      deleteAlarm(tempAlarm);

                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    if(tempAlarm->trigger == NULL || strcmp(tempAlarm->action, "") == 0){
                      //printf("INV ALARM\r\n" );
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
                  //printf("mallformed end\r\n");
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
                    //printf("doop version\r\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;
                    free(readLine);
                    fclose(fp);
                    return DUP_VER;
                  }

                  if(tempStr == NULL){
                    //printf("INVALUD VERSION\r\n");
                    deleteCalendar(tempCal);
                    (*obj) = NULL;
                    free(readLine);
                    fclose(fp);
                    return INV_VER;
                  }
                  //atof will default to 0.0 when a string cant be converted to float
                  tempCal->version = atof(tempStr);

                  if(tempCal->version == 0.0){
                    //printf("INVALUD VERSION\r\n");
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
                    //printf("DOOP prodid\r\n");
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
                  ////printf("proid is %s\r\n",tempCal->prodID);
                  break;
                }else{
                  //create a property and add it to the propertie list in Caledar object
                  if(addProperty(tempStr,&tempCal) == INV_CAL){
                    //printf("inv cal\r\n" );
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
                  if(tempStr == NULL){
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);
                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }
                  //check if UID already exists
                  if(strcmp(tempEvent->UID,"")!=0){
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);
                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }
                  strcpy(tempEvent->UID,tempStr);
                  break;
                }else if(strcmp(tempStr,"DTSTAMP") == 0){
                  tempStr = strtok(NULL,"");
                  if(tempStr == NULL){
                    //printf("inv event\r\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);
                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }

                  //check if creation date already exitsts
                  if(strcmp(tempEvent->creationDateTime.date,"")!=0
                  ||strcmp(tempEvent->creationDateTime.time,"")!=0){
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
                    //printf("inv event\r\n" );
                    (*obj) = NULL;
                    deleteCalendar(tempCal);
                    deleteEvent(tempEvent);
                    free(readLine);
                    fclose(fp);
                    return INV_EVENT;
                  }

                  //check if datetime already exitsts
                  if(strcmp(tempEvent->startDateTime.date,"")!=0 ||strcmp(tempEvent->startDateTime.time,"")!=0){
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
                    //check if action already exists
                    if(tempStr == NULL || strcmp(tempAlarm->action,"")!=0){
                      printf("INV ALARM\r\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteAlarm(tempAlarm);
                      deleteEvent(tempEvent);
                      free(readLine);
                      fclose(fp);
                      return INV_ALARM;
                    }
                    strcpy(tempAlarm->action,tempStr);

                  }else if(strcmp(tempStr,"TRIGGER") == 0){
                    tempStr = strtok(NULL,"");
                    //check if trigger already exists
                    if(tempStr == NULL || tempAlarm->trigger != NULL){
                      printf("2INV ALARM\r\n" );
                      (*obj) = NULL;
                      deleteCalendar(tempCal);
                      deleteAlarm(tempAlarm);
                      deleteEvent(tempEvent);
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
                      deleteAlarm(tempAlarm);
                      deleteEvent(tempEvent);
                      fclose(fp);
                      return INV_ALARM;
                    }
                  }
                break;
                //error has occurred
                // default:
                // //printf("ERROR ICALENDAR OBJECT BEGAN INCORRECT\r\n");
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
      ////printf("last line is \"%s\"\r\n",readLine);
      tempStr = strtok(readLine,":;");
      //check if last line is
      if(strcmp(tempStr,"END")==0){
        tempStr = strtok(NULL,"");

        if(tempStr == NULL){
          //printf("mallformed end\r\n");
          //set temp to a bad string so it will reach proper error
          tempStr = "";
        }

        switch (objectLevel) {
          //calendar object should be ending
          case 1:
            if(strcmp(tempStr,"VCALENDAR")!=0){
              //printf("1inv cal\r\n");
              (*obj) = NULL;
              deleteCalendar(tempCal);
              free(readLine);
              fclose(fp);
              return INV_CAL;
            }
            if(tempCal->version == 0.00 || strcmp(tempCal->prodID,"")==0){
              //printf("1inv cal\r\n");
              (*obj) = NULL;
              deleteCalendar(tempCal);
              free(readLine);
              fclose(fp);
              return INV_CAL;
            }
          break;
          case 2:

            if(strcmp(tempStr,"VEVENT")!=0){
              //printf("inv event\r\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);
              free(readLine);
              fclose(fp);
              return INV_EVENT;
            }
            if(strcmp(tempEvent->UID,"")==0 || isValidDateTime(tempEvent->creationDateTime) == 1 ||
          isValidDateTime(tempEvent->startDateTime)== 1){
              //printf("inv event\r\n" );
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
              //printf("INV ALARM\r\n" );
              (*obj) = NULL;
              deleteCalendar(tempCal);
              deleteEvent(tempEvent);
              deleteAlarm(tempAlarm);
              free(readLine);
              fclose(fp);
              return INV_ALARM;
            }
            if(tempAlarm->trigger == NULL || strcmp(tempAlarm->action, "") == 0){
              //printf("INV ALARM\r\n" );
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
          //printf("mallformed end\r\n");
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
      //printf("inv cal\r\n");
      (*obj) = NULL;
      deleteCalendar(tempCal);
      deleteEvent(tempEvent);
      deleteAlarm(tempAlarm);
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
    printf("FILE:%s\n",fileName );

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

  if(obj == NULL) return;

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
      return "Calendar Empty\r\n";
  } pointer to a Calendar struct
**/
char* printCalendar(const Calendar* obj){
  char *toRtrn;
  char *temp;
  void* elem;
  ListIterator iter;

  if(obj == NULL) return NULL;


//  initilizes values to 0, unlike malloc.
  toRtrn = calloc(100,sizeof(char));
  temp = calloc(100,sizeof(char));

  strcpy(toRtrn,"\r\nBEGIN:VCALENDAR\r\n");
  sprintf(temp,"VERSION:%.2f\r\n",obj->version);
  //+14 to not get valgrind errors, unsure why must ask TA, its because sizeof() returns 8
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"PRODID:%s\r\n",obj->prodID);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack
	iter = createIterator(obj->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

	while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}

  iter = createIterator(obj->events);
  while ((elem = nextElement(&iter)) != NULL){
    Event* tempEvent = (Event*)elem;
    temp = printEvent(tempEvent);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +17));
  strcat(toRtrn,"END:VCALENDAR\r\n");
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
      strcpy(toRtrn,"PROCEDURE WAS OK\r\n");
    break;
    case INV_FILE:
      strcpy(toRtrn,"INVALID FILE\r\n");
    break;
    case INV_CAL:
      strcpy(toRtrn,"INVALID CALENDAR\r\n");
    break;
    case INV_VER:
      strcpy(toRtrn,"INVALID VERSION\r\n");
    break;
    case DUP_VER:
      strcpy(toRtrn, "DUPLICATE VERSION APPEARED\r\n");
    break;
    case INV_PRODID:
      strcpy(toRtrn,"INVALID PRODUCT ID\r\n");
    break;
    case DUP_PRODID:
      strcpy(toRtrn,"DUPULICATE PRODUCT ID\r\n");
    break;
    case INV_EVENT:
      strcpy(toRtrn,"INVALID EVENT\r\n");
    break;
    case INV_DT:
      strcpy(toRtrn,"INVALID DATETIME\r\n");
    break;
    case INV_ALARM:
      strcpy(toRtrn, "INVALID ALARM\r\n");
    break;
    case WRITE_ERROR:
      strcpy(toRtrn, "WRITE ERROR\r\n");
    break;
    case OTHER_ERROR:
      strcpy(toRtrn,"OTHER ERROR OCCURED\r\n");
    break;
    default:
      strcpy(toRtrn,"UNKOWN ERROR OCCURED\r\n");
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
    return WRITE_ERROR;
  }

  if(obj == NULL){
    return INV_CAL;
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
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +17));
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


  if(obj == NULL) return INV_CAL;

  //ensure Calendar has UID, creation/start date, and at least 1 event
  if(obj->events == NULL) return INV_CAL;
  if(strcmp(obj->prodID,"")== 0) return INV_CAL;
  if(obj->version <= 0) return INV_CAL;
  if(getFromFront(obj->events) == NULL) return INV_CAL;
  if(obj->properties == NULL) return INV_CAL;


  iter = createIterator(obj->events);
  //check if all events are valid
  while ((elem = nextElement(&iter)) != NULL){
    Event* tempEvent = (Event*)elem;
    if(strcmp(tempEvent->UID, "")==0) return INV_EVENT;
    //check if dates are valid
    if(isValidDateTime(tempEvent->creationDateTime) ==1)return INV_EVENT;
    if(isValidDateTime(tempEvent->startDateTime)==1) return INV_EVENT;

    if(tempEvent->alarms == NULL ) return INV_EVENT;

    iterTwo = createIterator(tempEvent->alarms);
    //check if all alarms are valid within an event
    while ((elemTwo = nextElement(&iterTwo))!=NULL){
      Alarm* tempAlarm = (Alarm*)elemTwo;
      if(strcmp(tempAlarm->action,"")==0)return INV_ALARM;
      //check trigger is not null and not blank
      if(tempAlarm->trigger == NULL) return INV_ALARM;
      if(strcmp(tempAlarm->trigger,"")==0)return INV_ALARM;

      //check all properties are valid within alarm
      if(tempAlarm->properties == NULL) return INV_ALARM;

      iterThree = createIterator(tempAlarm->properties);
      while((elemThree = nextElement(&iterThree)) != NULL){
        Property* tempProp = (Property*)elemThree;
        if(isValidAlarmProperty(tempProp,tempAlarm) == 0) return INV_ALARM;
      }
    }

    if(tempEvent->properties == NULL ) return INV_EVENT;

    //check Event properties for validity
    iterThree = createIterator(tempEvent->properties);
    while((elemThree = nextElement(&iterThree)) != NULL){
      Property* tempProp = (Property*)elemThree;
      if(isValidEventProperty(tempProp,tempEvent) == 0) return INV_EVENT;
    }
  }

  //check if properties are valid and list does not contain NULL pointers.
  //doesnt make sense that there could be null pointers but whatever
  iterThree = createIterator(obj->properties);
  while((elemThree = nextElement(&iterThree)) != NULL){
    Property* tempProp = (Property*)elemThree;
    if(isValidProperty(tempProp) == 0) return INV_CAL;
  }

  return OK;
}



/** Function to converting a DateTime into a JSON string
 *@pre N/A
 *@post DateTime has not been modified in any way
 *@return A string in JSON format
 *@param prop - a DateTime struct
 **/
char* dtToJSON(DateTime prop){
  char* toRtrn;
  char boolString[6];
  if(prop.UTC == true){
    strcpy(boolString,"true");
  }else{
    strcpy(boolString,"false");
  }
  toRtrn = malloc(sizeof(char)*256);
  sprintf(toRtrn,"{\"date\":\"%s\",\"time\":\"%s\",\"isUTC\":%s}",prop.date,prop.time,boolString);

  return toRtrn;
}

/** Function to converting an Event into a JSON string
 *@pre Event is not NULL
 *@post Event has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to an Event struct
 **/
char* eventToJSON(const Event* event){
  char *toRtrn;
  char *dt;
  char *summary = NULL;
  ListIterator iter;
  void* elem;
  int propCount,alarmCount;

  if(event == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"{}");
    return toRtrn;
  }
  dt = dtToJSON(event->startDateTime);
  //look for summary propertie
  iter = createIterator(event->properties);
  while ((elem = nextElement(&iter)) != NULL){
      Property* tempProp = (Property*)elem;
      char *pName = tempProp->propName;
      strUpper(pName);
      if(strcmp(pName,"SUMMARY")==0){
        summary = malloc(sizeof(char)*strlen(tempProp->propDescr)+1);
        strcpy(summary,tempProp->propDescr);
        break;
      }
  }
  if(summary == NULL){
    summary = malloc(sizeof(char)*2);
    strcpy(summary,"");
  }

  propCount = event->properties->length +3;
  alarmCount = event->alarms->length;
  toRtrn = malloc(sizeof(char)*256);
  sprintf(toRtrn,"{\"startDT\":%s,\"numProps\":%d,\"numAlarms\":%d,\"summary\":\"%s\"}",dt,propCount,alarmCount,summary);

  free(summary);
  free(dt);

  return toRtrn;
}

/** Function to converting an Event list into a JSON string
 *@pre Event list is not NULL
 *@post Event list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* eventListToJSON(const List* eventList){
  char *toRtrn;
  char *temp = NULL;
  char *temp2=NULL;

  ListIterator iter;
  void* elem;

  if(eventList == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  iter = createConstIterator(eventList);
  elem = nextElement(&iter);
  Event* tempEvent = (Event*)elem;
  if(elem == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  toRtrn = eventToJSON(tempEvent);
  //defiantly could of designed this better but its fine
  //iterate though events adding a comma to begging of JSON string then
  //concatonating to end of return string
  while ((elem = nextElement(&iter)) != NULL){
    tempEvent = (Event*)elem;
    temp2=malloc(sizeof(char)*3);
    strcpy(temp2,",");
    temp = eventToJSON(tempEvent);
    temp2 = realloc(temp2,sizeof(char)*strlen(temp)+3);
    strcat(temp2,temp);

    toRtrn = realloc(toRtrn,strlen(toRtrn) + strlen(temp2)+1);
    strcat(toRtrn,temp2);
  }

  //toRtrn = realloc(toRtrn,strlen(toRtrn)+4);
  temp = realloc(temp,sizeof(char)*strlen(toRtrn)+4);
  sprintf(temp,"[%s]",toRtrn);

  free(toRtrn);
  free(temp2);
  return temp;
}

/** Function to converting a Calendar into a JSON string
 *@pre Calendar is not NULL
 *@post Calendar has not been modified in any way
 *@return A string in JSON format
 *@param cal - a pointer to a Calendar struct
 **/
char* calendarToJSON(const Calendar* cal){
  char *toRtrn;
  char *prodIDVal = NULL;
  float ver;
  int numProps,numEvents;

  if(cal == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"{}");
    return toRtrn;
  }
  prodIDVal = malloc(sizeof(char)*201);
  strcpy(prodIDVal,cal->prodID);
  ver = cal->version;
  numProps = getLength(cal->properties) +2;
  numEvents = getLength(cal->events);

  toRtrn = malloc(sizeof(char)*256);
  sprintf(toRtrn,"{\"version\":%.0f,\"prodID\":\"%s\",\"numProps\":%d,\"numEvents\":%d}",ver,prodIDVal,numProps,numEvents);

  free(prodIDVal);
  return toRtrn;
}

/** Function to converting a JSON string into a Calendar struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Calendar struct
 *@param str - a pointer to a string
 **/
Calendar* JSONtoCalendar(const char* str){
  Calendar *tempCal;
  float ver = 0;
  char *tempProdID = malloc(sizeof(char)*1001);
  const char *format = "{\"version\":%f,\"prodID\":\"%[^\"]\"}";

  if(str == NULL) return NULL;
  if(strcmp(str,"")==0) return NULL;

  if(sscanf(str,format,&ver,tempProdID) != 2){

    free(tempProdID);
    return NULL;
  }

  tempCal = calloc(1,sizeof(Calendar));
  tempCal->version = ver;
  strcpy(tempCal->prodID,tempProdID);
  tempCal->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
  tempCal->events = initializeList(&printEvent,&deleteEvent,&compareEvents);

  free(tempProdID);
  return tempCal;
}

/** Function to converting a JSON string into an Event struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and partially initialized Event struct
 *@param str - a pointer to a string
 **/
Event* JSONtoEvent(const char* str){
  Event *tempEvent;
  char *tempUID = malloc(sizeof(char)*1001);
  //value can have up too 1000 digits
  const char *format = "{\"UID\":\"%[^\"]\"}";
  if(str == NULL) return NULL;
  if(strcmp(str,"")==0) return NULL;

  if(sscanf(str,format,tempUID) != 1){
    free(tempUID);
    return NULL;
  }

  tempEvent = calloc(1,sizeof(Event));
  strcpy(tempEvent->UID,tempUID);
  tempEvent->properties = initializeList(&printProperty,&deleteProperty,&compareProperties);
  tempEvent->alarms = initializeList(&printAlarm,&deleteAlarm,&compareAlarms);

  free(tempUID);
  return tempEvent;
}

/** Function to adding an Event struct to an ixisting Calendar struct
 *@pre arguments are not NULL
 *@post The new event has been added to the calendar's events list
 *@return N/A
 *@param cal - a Calendar struct
 *@param toBeAdded - an Event struct
 **/
void addEvent(Calendar* cal, Event* toBeAdded){

  if(toBeAdded == NULL) return;
  if(cal == NULL) return;

  insertBack(cal->events,toBeAdded);

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
  //printf("deleting %s\r\n",printProperty(tempProp) );
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

  strcpy(toRtrn,"BEGIN:VEVENT\r\n");
  sprintf(temp,"UID:%s\r\n",tempEvent->UID);

  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  tempDT = printDate(&(tempEvent->creationDateTime));
  sprintf(temp,"DTSTAMP:%s\r\n",tempDT);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);
  free(tempDT);

  tempDT = printDate(&(tempEvent->startDateTime));
  sprintf(temp,"DTSTART:%s\r\n",tempDT);
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
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}

  iter = createIterator(tempEvent->alarms);
  while ((elem = nextElement(&iter)) != NULL){
    Alarm* tempAlarm = (Alarm*)elem;
    temp = printAlarm(tempAlarm);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}


  toRtrn = realloc(toRtrn, (strlen(toRtrn) +14));
  strcat(toRtrn,"END:VEVENT");

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
  strcpy(toRtrn,"BEGIN:VALARM\r\n");
  sprintf(temp,"ACTION:%s\r\n",tempAlarm->action);

  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  sprintf(temp,"TRIGGER:%s\r\n",tempAlarm->trigger);
  toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
  strcat(toRtrn,temp);

  //Create an iterator the iterator is allocated on the stack


	iter = createIterator(tempAlarm->properties);
  //free temp because we're adding the string created by printProperty now
  free(temp);

	while ((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;
    temp = printProperty(tempProp);
    strcat(temp,"\r\n");
    toRtrn = realloc(toRtrn, (strlen(toRtrn) + strlen(temp))+1);
    strcat(toRtrn,temp);
    free(temp);
	}
  toRtrn = realloc(toRtrn, (strlen(toRtrn) +14));
  strcat(toRtrn,"END:VALARM");

  return toRtrn;
}


void deleteProperty(void* toBeDeleted){
  Property *tempProp;

  if (toBeDeleted == NULL){
		return;
	}
  tempProp = (Property*)toBeDeleted;
  //printf("deleting %s\r\n",printProperty(//tempProp) );
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
  if(strcmp(tempProp->propName,"ATTACH")==0){
    sprintf(toRtrn,"%s;%s",tempProp->propName,tempProp->propDescr);

  }else{

    sprintf(toRtrn,"%s:%s",tempProp->propName,tempProp->propDescr);
  }


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
    //name will be set to "" in beginning of dealing with line
    if(strcmp(property,"")==0){
      return INV_CAL;
    }

    Property *newProperty = malloc(sizeof(Property));
    //char *tmp;
    strcpy(newProperty->propName,property);
    property = strtok(NULL,"");

    if(property == NULL){
      deleteProperty(newProperty);
      return INV_CAL;
    }
    //realloc to account for size of description becuase of dynamic array in Property struct
    newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
    strcpy(newProperty->propDescr,property);

    // tmp = printProperty(newProperty);
    // ////printf("property to create: %s\r\n",tmp);
    // free(tmp);

    insertFront((*obj)->properties,newProperty);

    return OK;
}

ICalErrorCode addPropertyEvent(char *property, Event** obj){
  if(strcmp(property,"")==0){
    return INV_EVENT;
  }
  Property *newProperty = malloc(sizeof(Property));
  //  char *tmp;
  strcpy(newProperty->propName,property);

  //will turn the rest of the tokenized thing into the description of property
  property = strtok(NULL,"");

  if(property == NULL){
    deleteProperty(newProperty);
    return INV_EVENT;
  }
  //realloc to account for size of description becuase of dynamic array in Property struct
  newProperty = realloc(newProperty,(sizeof(Property) + sizeof(char)*strlen(property)+1));
  strcpy(newProperty->propDescr,property);

  insertFront((*obj)->properties,newProperty);

  return OK;
}

ICalErrorCode addPropertyAlarm(char *property, Alarm** obj){
  if(strcmp(property,"")==0){
    return INV_ALARM;
  }

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

    return 0;
}

//checks if input propertie is valud for calendar properties
int isValidProperty(Property *obj){
  char* toCompare;
  int len;

  if(obj == NULL) return 0;

  if(strcmp(obj->propName,"") == 0) return 0;

  if(strcmp(obj->propDescr,"")==0) return 0;

  len = strlen(obj->propName);
  toCompare = malloc(sizeof(char)*len+1);
  strcpy(toCompare, obj->propName);
  strUpper(toCompare);

  //can only be two properties but may occur more than once
  //so we only need to check for valid name
  if(strcmp(toCompare,"CALSCALE")==0 || strcmp(toCompare,"METHOD")==0){
    free(toCompare);
    return 1;
  }


  free(toCompare);
  return 0;
}

//checks event properties
int isValidEventProperty(Property *obj,Event *tempEvent){
  char *propDes[100];
  char *toCompare;
  ListIterator iter;
  void* elem;
  int i,len;

  if(obj == NULL) return 0;

  if(strcmp(obj->propName,"") == 0) return 0;

  if(strcmp(obj->propDescr,"")==0) return 0;

  len = strlen(obj->propName);
  toCompare = malloc(sizeof(char)*len+1);
  strcpy(toCompare, obj->propName);
  strUpper(toCompare);


    //add all valid property names to list
    propDes[0] = "CLASS";

    propDes[1] = "CREATED";

    propDes[2] = "DESCRIPTION";

    propDes[3] = "GEO";

    propDes[4] = "LAST-MODIFIED";

    propDes[5] = "LOCATION";

    propDes[6] = "ORGANIZER";

    propDes[7] = "PRIORITY";

    propDes[8] = "SEQ";

    propDes[9] = "STATUS";

    propDes[10] = "SUMMARY";

    propDes[11] = "TRANSP";

    propDes[12] = "URL";

    propDes[13] = "RECURID";

    propDes[14] = "ATTACH";

    propDes[15] = "ATTENDEE";

    propDes[16] = "CATEGORIES";

    propDes[17] = "COMMENT";

    propDes[18] = "CONTACT";

    propDes[19] = "EXDATE";

    propDes[20] = "RSTATUS";

    propDes[21] = "RELATED";

    propDes[22] = "RESOURCES";

    propDes[23] = "RDATE";

    propDes[24] = "DTEND";

    propDes[25] = "DURATION";

    //check if event property has already occured once
    iter = createIterator(tempEvent->properties);
    while((elem = nextElement(&iter)) != NULL){
      Property* tempProp = (Property*)elem;

      //ignore when both are pointing to same object
      if(obj != tempProp ){
        if(strcmp(obj->propDescr,tempProp->propDescr) == 0
        && strcmp(obj->propName,tempProp->propName) == 0){
          // printf("des 1:%s:  2:%s:\n", obj->propDescr,tempProp->propDescr);
          // printf("name 1:%s:  2:%s:\n", obj->propName,tempProp->propName);
          free(toCompare);
          return 0;
        }
      }
    }

    for(i=0;i<26;i++){
      if(strcmp(toCompare,propDes[i])==0){
        free(toCompare);
        return 1;
      }
    }
  free(toCompare);
  return 0;
}

//sends string to all upper case for easier comparing
void strUpper(char string[]){
	int i = 0;

	while (string[i] != '\0')
	{
    	if (string[i] >= 'a' && string[i] <= 'z') {
        	string[i] = string[i] - 32;
    	}
      	i++;
	}
}

int isValidAlarmProperty(Property *obj,Alarm *tempAlarm){
  char *toCompare;
  char *propDes[100];
  int i;
  ListIterator iter;
  void* elem;

  if(obj == NULL) return 0;

  if(strcmp(obj->propName,"") == 0) return 0;

  if(strcmp(obj->propDescr,"")==0) return 0;
  toCompare = malloc(sizeof(char)*strlen(obj->propName)+1);
  strcpy(toCompare, obj->propName);
  strUpper(toCompare);

  propDes[0] = "DURATION";
  propDes[1] = "REPEAT";
  propDes[2] = "ATTACH";


  //checks for repeat properties
  iter = createIterator(tempAlarm->properties);
  while((elem = nextElement(&iter)) != NULL){
    Property* tempProp = (Property*)elem;

    //ignore when both are pointing to same object or item is attach because there can be multiple
    if(obj != tempProp && strcmp(toCompare,"ATTACH") != 0 ){
      if(strcmp(obj->propDescr,tempProp->propDescr) == 0
      && strcmp(obj->propName,tempProp->propName) == 0){
        // printf("des 1:%s:  2:%s:\n", obj->propDescr,tempProp->propDescr);
        // printf("name 1:%s:  2:%s:\n", obj->propName,tempProp->propName);
        free(toCompare);
        return 0;
      }
    }
  }

  //must be one of the allowed properties
  for(i =0;i<3;i++){
    if(strcmp(toCompare,propDes[i])==0){
      free(toCompare);
      return 1;
    }
  }


  free(toCompare);
  return 0;
}

//function to help with eventListToJSON
ListIterator createConstIterator(const List* list){
    ListIterator iter;

    iter.current = list->head;

    return iter;
}



char* alarmToJSON(Alarm *prop){
  char* toRtrn;
  if(prop == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"{}");
    return toRtrn;
  }
  toRtrn = malloc(sizeof(char)*256);
  sprintf(toRtrn,"{\"action\":\"%s\",\"trigger\":\"%s\",\"numProps\":%d}",prop->action,prop->trigger,prop->properties->length+2);

  return toRtrn;
}

char* propertyToJSON(Property *prop){
  char* toRtrn;
  if(prop == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"{}");
    return toRtrn;
  }
  toRtrn = malloc(sizeof(char)*256);
  sprintf(toRtrn,"{\"propName\":\"%s\",\"propDescr\":\"%s\"}",prop->propName,prop->propDescr);

  return toRtrn;
}



/** Function to converting an Event list into a JSON string
 *@pre Property list is not NULL
 *@post Property list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* propertieListToJSON(const List* propertyList){
  char *toRtrn;
  char *temp = NULL;
  char *temp2=NULL;

  ListIterator iter;
  void* elem;

  if(propertyList == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  iter = createConstIterator(propertyList);
  elem = nextElement(&iter);
  Property* tempProp = (Property*)elem;
  if(elem == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  toRtrn = propertyToJSON(tempProp);
  //defiantly could of designed this better but its fine
  //iterate though events adding a comma to begging of JSON string then
  //concatonating to end of return string
  while ((elem = nextElement(&iter)) != NULL){
    tempProp = (Property*)elem;
    temp2=malloc(sizeof(char)*3);
    strcpy(temp2,",");
    temp = propertyToJSON(tempProp);
    temp2 = realloc(temp2,sizeof(char)*strlen(temp)+3);
    strcat(temp2,temp);

    toRtrn = realloc(toRtrn,strlen(toRtrn) + strlen(temp2)+1);
    strcat(toRtrn,temp2);
  }

  //toRtrn = realloc(toRtrn,strlen(toRtrn)+4);
  temp = realloc(temp,sizeof(char)*strlen(toRtrn)+4);
  sprintf(temp,"[%s]",toRtrn);

  free(toRtrn);
  free(temp2);
  return temp;
}

/** Function to converting an Event list into a JSON string
 *@pre Event list is not NULL
 *@post Event list has not been modified in any way
 *@return A string in JSON format
 *@param eventList - a pointer to an Event list
 **/
char* alarmListToJSON(const List* alarmList){
  char *toRtrn;
  char *temp = NULL;
  char *temp2=NULL;

  ListIterator iter;
  void* elem;

  if(alarmList == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  iter = createConstIterator(alarmList);
  elem = nextElement(&iter);
  Alarm* tempAlarm = (Alarm*)elem;
  if(elem == NULL){
    toRtrn = malloc(sizeof(char)*3);
    strcpy(toRtrn,"[]");
    return toRtrn;
  }

  toRtrn = alarmToJSON(tempAlarm);
  //defiantly could of designed this better but its fine
  //iterate though events adding a comma to begging of JSON string then
  //concatonating to end of return string
  while ((elem = nextElement(&iter)) != NULL){
    tempAlarm = (Alarm*)elem;
    temp2=malloc(sizeof(char)*3);
    strcpy(temp2,",");
    temp = alarmToJSON(tempAlarm);
    temp2 = realloc(temp2,sizeof(char)*strlen(temp)+3);
    strcat(temp2,temp);

    toRtrn = realloc(toRtrn,strlen(toRtrn) + strlen(temp2)+1);
    strcat(toRtrn,temp2);
  }

  //toRtrn = realloc(toRtrn,strlen(toRtrn)+4);
  temp = realloc(temp,sizeof(char)*strlen(toRtrn)+4);
  sprintf(temp,"[%s]",toRtrn);

  free(toRtrn);
  free(temp2);
  return temp;
}

/**Node JS Helper Fucntions because idk how you use enums in js**/
Calendar* nodeCreateCal(char* fileName){
    Calendar *tempCal = NULL;
    ICalErrorCode error = createCalendar(fileName,&tempCal);
    if(tempCal != NULL){
       error = validateCalendar(tempCal);
    }
    char *temp = printError(error);
    if(error != OK){
        deleteCalendar(tempCal);
        tempCal = NULL;
        fprintf(stderr, "FAILED: %s\n",temp);
    }
    free(temp);

    return tempCal;
}

void nodeWriteCAl(char* fileName, Calendar* obj){
    ICalErrorCode error = writeCalendar(fileName, obj);
    if(error != OK){
        fprintf(stderr, "WRITE FILE FAILED\n");
    }
}

char* nodeEventListJSON(Calendar* obj){
  char *json;
  ICalErrorCode error = validateCalendar(obj);
  if(error != OK){
     fprintf(stderr, "FAILED eventListToJSON\n");
   }
  json = eventListToJSON(obj->events);

  return json;
}

char* nodePropertieListJSON(char *fileName, int evntNum){
  char *json;
  Calendar *tempCal = NULL;
  Event *tempEvent = NULL;
  ListIterator iter;
  void* elem;
  int i =1;

  fprintf(stderr, "PropertieListJSON file name is %s, evt is %d\n",fileName,evntNum);

  ICalErrorCode error = createCalendar(fileName,&tempCal);
  if(tempCal != NULL){
     error = validateCalendar(tempCal);
  }
  if(error != OK){
      fprintf(stderr, "FAILED PropertieListJSON\n");
      return NULL;
  }

  iter = createConstIterator(tempCal->events);
  elem = nextElement(&iter);
  tempEvent = (Event*)elem;
  if(elem == NULL){
    json = malloc(sizeof(char)*3);
    strcpy(json,"[]");
    return json;
  }

  //goes through and finds event
  while (((elem = nextElement(&iter)) != NULL) && i< evntNum){
    tempEvent = (Event*)elem;
    i++;
  }

  json = propertieListToJSON(tempEvent->properties);

  return json;
}

/*
creats a string for all alarms for passed in file and selected event
*/
char* nodeAlarmListJSON(char *fileName, int evntNum){
  char *json;
  Calendar *tempCal = NULL;
  Event *tempEvent = NULL;
  ListIterator iter;
  void* elem;
  int i =1;

  fprintf(stderr, "PropertieListJSON file name is %s, evt is %d\n",fileName,evntNum);

  ICalErrorCode error = createCalendar(fileName,&tempCal);

  if(tempCal != NULL){
     error = validateCalendar(tempCal);
  }
  if(error != OK){
      fprintf(stderr, "FAILED PropertieListJSON\n");
      return NULL;
  }

  iter = createConstIterator(tempCal->events);
  elem = nextElement(&iter);
  tempEvent = (Event*)elem;
  if(elem == NULL){
    json = malloc(sizeof(char)*3);
    strcpy(json,"[]");
    return json;
  }

  fprintf(stderr, "event UID %s i=%d\n",tempEvent->UID,i);
  //goes through and finds event
  while (((elem = nextElement(&iter)) != NULL) && i < evntNum){
    tempEvent = (Event*)elem;
    fprintf(stderr, "event UID %s i=%d\n",tempEvent->UID,i);

    i++;
  }
fprintf(stderr, "event UID %s i=%d\n",tempEvent->UID,i);
  json = alarmListToJSON(tempEvent->alarms);

  return json;

}
