//-----------------------------------------------
// CsDateProperties.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System;

class CsDateProperties
{
     public static void Main()
     {
          Date mydate = new Date();

          try
          {
               mydate.Month = 8;
               mydate.Day   = 29;
               mydate.Year  = 2001;

               Console.WriteLine("Day of year = {0}", mydate.DayOfYear);
          }
          catch (Exception exc)
          {
               Console.WriteLine(exc);
          }
     }         
}
class Date
{
                                                            // Fields
     int year;
     int month;
     int day;
     static int[] MonthDays = new int[] {   0,  31,  59,  90, 120, 151,
	 							          181, 212, 243, 273, 304, 334 };

                                                            // Properties
     public int Year
     {
          set
          {
               if (value < 1600)
                    throw new ArgumentOutOfRangeException("Year");
               else
                    year = value;
          }
          get
          {
               return year;
          }
     }
     public int Month
     {
          set
          {
               if (value < 1 || value > 12)
                    throw new ArgumentOutOfRangeException("Month");
               else
                    month = value;
          }
          get
          {
               return month;
          }
     }
     public int Day
     {
          set
          {
               if (value < 1 || value > 31)
                    throw new ArgumentOutOfRangeException("Day");
               else
                    day = value;
          }
          get
          {
               return day;
          }
     }
     public int DayOfYear
     {
          get
          {
               return MonthDays[month - 1] + day + 
                                   (month > 2 && IsLeapYear(year) ? 1 : 0);
          }
     }
                                                            // Method
     public static bool IsLeapYear(int year)
     {
          return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
     }
}