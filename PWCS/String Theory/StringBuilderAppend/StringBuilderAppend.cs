//--------------------------------------------------
// StringBuilderAppend.cs © 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Text;

class StringBuilderAppend
{
     const int iIterations = 10000;

     public static void Main()
     {
          DateTime      dt = DateTime.Now;
          StringBuilder sb = new StringBuilder();

          for (int i = 0; i < iIterations; i++)
               sb.Append("abcdefghijklmnopqrstuvwxyz\r\n");

          string str = sb.ToString();

          Console.WriteLine(DateTime.Now - dt);
     }
}