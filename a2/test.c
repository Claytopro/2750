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
  char* temp;

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
          free(calendarToPtr);
          temp = printError(validateCalendar(tester));
          printf("validate Calendar:%s\n", temp);
          free(temp);

          printf("--------TESTING DT TO JSON---\n");

          iter = createIterator(tester->events);
        	while ((elem = nextElement(&iter)) != NULL){
          Event* tempEvent = (Event*)elem;
            temp = eventToJSON(tempEvent);
            printf("JSONT DT TEST:%s\n",temp);
            free(temp);
        	}
          temp = eventListToJSON(tester->events);
          printf("--------TESTING event list to JSON---\n%s\n\n",temp);
          free(temp);

          temp = calendarToJSON(tester);
          printf("--------TESTING CALENDAR to JSON---\n%s\n\n",temp);
          free(temp);

          writeCalendar("test5.ics",tester);

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
                free(calendarToPtr);
                temp = printError(validateCalendar(tester));
                printf("validate Calendar:%s\n", temp);
                free(temp);

                printf("--------TESTING EVENT JSON WITH SUMMARY---\n");

                iter = createIterator(tester->events);
              	while ((elem = nextElement(&iter)) != NULL){
                Event* tempEvent = (Event*)elem;
                  temp = eventToJSON(tempEvent);
                  printf("JSONT DT TEST:%s\n",temp);
                  free(temp);

              	}
                printf("break point\n");
                temp = eventListToJSON(tester->events);
                printf("--------TESTING event list to JSON w/ 1 event---\n%s\n\n",temp);
                free(temp);

                temp = calendarToJSON(tester);
                printf("--------TESTING CALENDAR to JSON with 1 event---\n%s\n\n",temp);
                free(temp);

                writeCalendar("test6.ics",tester);

                deleteCalendar(tester);
              }
            }


  return 0;
}
