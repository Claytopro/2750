/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"
#include "LinkedListAPI.c"



int main(){
  Calendar *tester = NULL;
  char *calendarToPtr;


  if(createCalendar("test.ics", &tester) != OK){
  //printf("error:%s:\n",printError(createCalendar("test.ics", &tester)) );

  }else{

    if(tester != NULL){
      calendarToPtr = printCalendar(tester);
      printf("%s\n",calendarToPtr);

      if(calendarToPtr != NULL){
        //  printf("Freeing calendarString\n" );
        free(calendarToPtr);
      }
      deleteCalendar(tester);
    }
  }

  if(createCalendar("test1.ics", &tester) != OK){
    if(tester != NULL){
      printf("calendar not set to null on erro\n");
    }
    printf("test1:baddd\n");
  //  deleteCalendar(tester);
  }else{

    if(tester != NULL){
      calendarToPtr = printCalendar(tester);
      printf("%s\n",calendarToPtr);

      if(calendarToPtr != NULL){
        //  printf("Freeing calendarString\n" );
        free(calendarToPtr);
      }
      deleteCalendar(tester);
    }
  }

  if(createCalendar("test2.ics", &tester) != OK){
    if(tester != NULL){
      printf("calendar not set to null on erro\n");
    }
    printf("test2:baddd\n");
  //  deleteCalendar(tester);
  }else{

    if(tester != NULL){
      calendarToPtr = printCalendar(tester);
      printf("%s\n",calendarToPtr);

      if(calendarToPtr != NULL){
        //  printf("Freeing calendarString\n" );
        free(calendarToPtr);
      }
      deleteCalendar(tester);
    }
  }


    if(createCalendar("test3.ics", &tester) != OK){
      if(tester != NULL){
        printf("calendar not set to null on erro\n");
      }
      printf("test3:baddd\n");
    //  deleteCalendar(tester);
    }else{

      if(tester != NULL){
        calendarToPtr = printCalendar(tester);
        printf("%s\n",calendarToPtr);

        //change some shit and test validate
        if(validateCalendar(tester) == OK) printf("VALIDATE CALENDAR:ok\n");
        tester->version =0;//kind of a shitty test
        if(validateCalendar(tester) != OK) printf("NOT OK VALIDATE CALENDAR:ok\n");



        if(calendarToPtr != NULL){
          //  printf("Freeing calendarString\n" );
          free(calendarToPtr);
        }
        deleteCalendar(tester);
      }
    }

printf("--------TESTING VALID EVENT PROPERTY---\n");
      if(createCalendar("test4.ics", &tester) != OK){
        if(tester != NULL){
          printf("calendar not set to null on erro\n");
        }
        printf("test4:baddd\n");
      //  deleteCalendar(tester);
      }else{

        if(tester != NULL){
          calendarToPtr = printCalendar(tester);
          printf("%s\n",calendarToPtr);
          printf("validate Calendar:%s\n", printError(validateCalendar(tester)));

          writeCalendar("test5.ics",tester);
          if(calendarToPtr != NULL){
            //  printf("Freeing calendarString\n" );
            free(calendarToPtr);
          }
          deleteCalendar(tester);
        }
      }

printf("--------TESTING VALID EVENT PROPERTY---\n");
      if(createCalendar("invEvtProp1.isc", &tester) != OK){
        if(tester != NULL){
          printf("calendar not set to null on erro\n");
        }
        printf("invalid event because of property not being allowed\n");
      }else{
        calendarToPtr = printCalendar(tester);
        printf("%s\n",calendarToPtr);
        if(calendarToPtr != NULL){
          //  printf("Freeing calendarString\n" );
          free(calendarToPtr);
        }
        deleteCalendar(tester);
      }

      printf("%s\n",printError(createCalendar("invEvtProp2.isc", &tester)) );

        if(tester == NULL){
          printf("calendar not set to null on erro\n");

        printf("invalid event because of property not being allowed\n");
      }else{
        calendarToPtr = printCalendar(tester);
        printf("%s\n",calendarToPtr);

        printf("validate Calendar:%s\n", printError(validateCalendar(tester)));
        if(calendarToPtr != NULL){
          //  printf("Freeing calendarString\n" );
          free(calendarToPtr);
        }
        deleteCalendar(tester);
      }


  return 0;
}
