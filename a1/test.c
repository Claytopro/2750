/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"



int main(){
  Calendar *tester;
  char *calendarToPtr;


  if(createCalendar("test.txt", &tester) != OK){
    if(tester != NULL){
      printf("calendar not set to null on erro\n");
    }
    printf("baddd\n");
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
