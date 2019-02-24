/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"
#include "LinkedListAPI.c"



int main(){
  Calendar *tester = NULL;
  char *calendarToPtr;
  void* elem;

  ListIterator iter;



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

          printf("--------TESTING DT TO JSON---\n");

          iter = createIterator(tester->events);
        	while ((elem = nextElement(&iter)) != NULL){
          Event* tempEvent = (Event*)elem;

            printf("JSONT DT TEST:%s\n",eventToJSON(tempEvent));

        	}


          writeCalendar("test5.ics",tester);
          if(calendarToPtr != NULL){
            //  printf("Freeing calendarString\n" );
            free(calendarToPtr);
          }
          deleteCalendar(tester);
        }
      }

      printf("--------TESTING VALID EVENT PROPERTY---\n");
            if(createCalendar("test3.ics", &tester) != OK){
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

                printf("--------TESTING EVENT JSON WITH SUMMARY---\n");

                iter = createIterator(tester->events);
              	while ((elem = nextElement(&iter)) != NULL){
                Event* tempEvent = (Event*)elem;

                  printf("JSONT DT TEST:%s\n",eventToJSON(tempEvent));

              	}


                writeCalendar("test6.ics",tester);
                if(calendarToPtr != NULL){
                  //  printf("Freeing calendarString\n" );
                  free(calendarToPtr);
                }
                deleteCalendar(tester);
              }
            }


  return 0;
}
