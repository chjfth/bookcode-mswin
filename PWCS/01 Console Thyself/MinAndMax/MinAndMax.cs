//---------------------------------------- 
// MinAndMax.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;

class MinAndMax
{
     public static void Main()
     {
          Console.WriteLine("sbyte:   {0} to {1}", sbyte.MinValue, 
                                                            sbyte.MaxValue);
          Console.WriteLine("byte:    {0} to {1}", byte.MinValue,    
                                                            byte.MaxValue);
          Console.WriteLine("short:   {0} to {1}", short.MinValue,   
                                                            short.MaxValue);
          Console.WriteLine("ushort:  {0} to {1}", ushort.MinValue,  
                                                            ushort.MaxValue);
          Console.WriteLine("int:     {0} to {1}", int.MinValue,     
                                                            int.MaxValue);
          Console.WriteLine("uint:    {0} to {1}", uint.MinValue,    
                                                            uint.MaxValue);
          Console.WriteLine("long:    {0} to {1}", long.MinValue,    
                                                            long.MaxValue);
          Console.WriteLine("ulong:   {0} to {1}", ulong.MinValue,   
                                                            ulong.MaxValue);
          Console.WriteLine("float:   {0} to {1}", float.MinValue,   
                                                            float.MaxValue);
          Console.WriteLine("double:  {0} to {1}", double.MinValue,  
                                                            double.MaxValue);
          Console.WriteLine("decimal: {0} to {1}", decimal.MinValue, 
                                                            decimal.MaxValue);
     }
}