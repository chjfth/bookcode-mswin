//-----------------------------------------------
// CsDateConstructors.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;

class CsDateConstructors
{
     public static void Main()
     {
          try
          {
               Date mydate = new Date(2001, 8, 29);

               Console.WriteLine("Day of year = " + mydate.DayOfYear);
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

	                                                        // Constructors
     public Date()
     {
          Year = 1600;
          Month = 1;
          Day = 1;
	 }
     public Date(int year, int month, int day)
     {
          if ( (month == 2 &&  IsLeapYear(year) && day > 29) ||
               (month == 2 && !IsLeapYear(year) && day > 28) ||
              ((month == 4 || month == 6 || 
                              month == 9 || month == 11) && day > 30))
          {
               throw new ArgumentOutOfRangeException("Day");
          }
          else
          {
               Year  = year;
               Month = month;
               Day   = day;
          }
     }
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