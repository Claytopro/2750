/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"



int main(){
  Calendar *tester;
  char *calendarToPtr;

  if(createCalendar("test.ics", &tester) != OK){
    printf("baddd\n");
    deleteCalendar(tester);
  }else{


    calendarToPtr = printCalendar(tester);
    printf("%s\n",calendarToPtr);

    if(calendarToPtr != NULL){
      //  printf("Freeing calendarString\n" );
      free(calendarToPtr);
    }

    deleteCalendar(tester);


  }



  return 0;
}
