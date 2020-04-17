//---------------------------------------------
// CppDateStruct.cpp © 2001 by Charles Petzold
//---------------------------------------------
#include <stdio.h>

struct Date
{
     int year;
     int month;
     int day;

     int IsLeapYear()
     {
          return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
     }
     int DayOfYear()
     {
          static int MonthDays[12] = {   0,  31,  59,  90, 120, 151,
                                       181, 212, 243, 273, 304, 334 };

          return MonthDays[month - 1] + day + ((month > 2) && IsLeapYear());
     }
};
int main(void)
{
	Date mydate;

	mydate.month = 8;
	mydate.day   = 29;
	mydate.year  = 2001;

	printf("Day of year = %i\n", mydate.DayOfYear());

	return 0;
}