/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"



int main(){
  Calendar *tester;
  char *calendarToPtr;

  createCalendar("test.ics", &tester);


  calendarToPtr = printCalendar(tester);
  printf("%s\n",calendarToPtr);

  if(calendarToPtr != NULL){
  //  printf("Freeing calendarString\n" );
    free(calendarToPtr);
  }

   deleteCalendar(tester);


  return 0;
}
