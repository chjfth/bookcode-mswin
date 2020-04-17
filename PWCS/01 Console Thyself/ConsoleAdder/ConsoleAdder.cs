//-------------------------------------------
// ConsoleAdder.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;

class ConsoleAdder
{
     public static void Main()
     {
          int a = 1509;
          int b = 744;
          int c = a + b;

          Console.Write("The sum of ");
          Console.Write(a);
          Console.Write(" and ");
          Console.Write(b);
          Console.Write(" equals ");
          Console.WriteLine(c);

          Console.WriteLine("The sum of " + a + " and " + b + " equals " + c);

          Console.WriteLine("The sum of {0} and {1} equals {2}", a, b, c);
     }
}