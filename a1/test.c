/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"

int main(){
  Calendar *tester;

  createCalendar("test.ics", &tester);

  printf("%s\n",printCalendar(tester));

  return 0;
}
