/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"
#include "LinkedListAPI.c"



int main(){
  Calendar *tester = NULL;
  Event *tempEvent;
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


            printf("--------JSONtoEvent---\n");
            tempEvent = JSONtoEvent("{\"UID\":\"123444\"}");
            printf("test 1:%s:\n",tempEvent->UID );
            deleteEvent(tempEvent);
            tempEvent = JSONtoEvent("{\"UID\":123aaa123\"}");
            printf("test 2:%s:\n",tempEvent->UID );
            deleteEvent(tempEvent);

            printf("--------JSONtoCalendar---\n");
            tester = JSONtoCalendar("{\"version\":2,\"prodID\":\"-//hacksw/handcal//NONSGML v1.0//EN\"}");
            printf("ver %.1f,proid :%s:\n",tester->version,tester->prodID);
            deleteCalendar(tester);


            printf("\n\n--------TESTING READ IN WRITE CALENDAR test5.ics---\n");
                  if(createCalendar("test5.ics", &tester) != OK){
                  printf("test5.ics is invalud\n");

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


                      deleteCalendar(tester);
                    }
                  }
  return 0;
}
