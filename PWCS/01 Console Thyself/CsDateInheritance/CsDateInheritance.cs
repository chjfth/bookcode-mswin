//------------------------------------------------
// CsDateInheritance.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;

class CsDateInheritance
{
     public static void Main()
     {
          DatePlus birth = new DatePlus(1953, 2, 2);
          DatePlus today = new DatePlus(2001, 8, 29);

          Console.WriteLine("Birthday = {0}", birth);
          Console.WriteLine("Today = " + today);
          Console.WriteLine("Days since birthday = {0}", today - birth);
     }         
}
class DatePlus: Date
{
     public DatePlus() {}
     public DatePlus(int year, int month, int day): base(year, month, day) {}

     public int DaysSince1600
     {
          get
          {
               return 365 * (Year - 1600) + 
                      (Year - 1597) / 4 -
                      (Year - 1601) / 100 +
                      (Year - 1601) / 400 + DayOfYear;
          }
     }
     public override string ToString()
     {
          string[] str = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

          return String.Format("{0} {1} {2}", Day, str[Month - 1], Year);
     }
     public static int operator -(DatePlus date1, DatePlus date2)
     {
          return date1.DaysSince1600 - date2.DaysSince1600;
     }
}