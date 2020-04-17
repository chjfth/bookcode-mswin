//-----------------------------------
// CDate.c © 2001 by Charles Petzold
//-----------------------------------
#include <stdio.h>

struct Date
{
	int year;
	int month;
	int day;
};
int IsLeapYear(int year)
{
	return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}
int DayOfYear(struct Date date)
{
     static int MonthDays[12] = {   0,  31,  59,  90, 120, 151,
                                  181, 212, 243, 273, 304, 334 };

     return MonthDays[date.month - 1] + date.day +
                            ((date.month > 2) && IsLeapYear(date.year));
}
int main(void)
{
	struct Date mydate;

	mydate.month = 8;
	mydate.day   = 29;
	mydate.year  = 2001;

	printf("Day of year = %i\n", DayOfYear(mydate));

	return 0;
}