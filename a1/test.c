/** Test cases for claendar calendar parser
*   By:Clayton Provan
*
**/

#include "CalendarParser.c"

int main(){
  Calendar *tester;

  createCalendar("test.ics", &tester);

  return 0;
}
