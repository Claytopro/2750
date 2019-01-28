/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"



int main(){
  Calendar *tester = NULL;
  char *calendarToPtr;


  if(createCalendar("test.ics", &tester) != OK){
    if(tester != NULL){
      printf("calendar not set to null on erro\n");
    }
    printf("test:baddd\n");
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

        if(calendarToPtr != NULL){
          //  printf("Freeing calendarString\n" );
          free(calendarToPtr);
        }
        deleteCalendar(tester);
      }
    }


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

          if(calendarToPtr != NULL){
            //  printf("Freeing calendarString\n" );
            free(calendarToPtr);
          }
          deleteCalendar(tester);
        }
      }


  return 0;
}
