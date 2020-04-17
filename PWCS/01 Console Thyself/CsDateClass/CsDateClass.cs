//------------------------------------------
// CsDateClass.cs © 2001 by Charles Petzold
//------------------------------------------
using System;

class CsDateClass
{
     public static void Main()
     {
          Date mydate = new Date();

          mydate.month = 8;
          mydate.day   = 29;
          mydate.year  = 2001;

          Console.WriteLine("Day of year = {0}", mydate.DayOfYear());
     }         
}
class Date
{
     public int year;
     public int month;
     public int day;

     public static bool IsLeapYear(int year)
     {
          return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
     }
     public int DayOfYear()
     {
          int[] MonthDays = new int[] {   0,  31,  59,  90, 120, 151,
                                        181, 212, 243, 273, 304, 334 };
          
          return MonthDays[month - 1] + day + 
                              (month > 2 && IsLeapYear(year) ? 1 : 0);
     }
}