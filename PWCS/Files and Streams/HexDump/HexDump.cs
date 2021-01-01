//--------------------------------------
// HexDump.cs (R) 2001 by Charles Petzold
//--------------------------------------
using System;
using System.IO;

class HexDump
{
     public static int Main(string[] astrArgs)
     {
          if (astrArgs.Length == 0)
          {
               Console.WriteLine("Syntax: HexDump file1 file2 ...");
               return 1;
          }
          foreach (string strFileName in astrArgs)
               DumpFile(strFileName);

          return 0;
     }
     protected static void DumpFile(string strFileName)
     {
          FileStream fs;

          try
          {
               fs = new FileStream(strFileName, FileMode.Open, 
                                   FileAccess.Read, FileShare.Read);
          }
          catch (Exception exc)
          {
               Console.WriteLine("HexDump: {0}", exc.Message);
               return;
          }
          Console.WriteLine(strFileName);
          DumpStream(fs);
          fs.Close();
     }
     protected static void DumpStream(Stream stream)
     {
          byte[] abyBuffer = new byte[16];
          long   lAddress  = 0;
          int    iCount;

          while ((iCount = stream.Read(abyBuffer, 0, 16)) > 0)
          {
               Console.WriteLine(ComposeLine(lAddress, abyBuffer, iCount));
               lAddress += 16;
          }
     }
     public static string ComposeLine(long lAddress, byte[] abyBuffer, 
                                      int iCount)
     {
          string str = String.Format("{0:X4}-{1:X4}  ",
                              (uint) lAddress / 65536, (ushort) lAddress);

          for (int i = 0; i < 16; i++)
          {
               str += (i < iCount) ? 
                              String.Format("{0:X2}", abyBuffer[i]) : "  ";
               str += (i == 7 && iCount > 7) ? "-" : " ";
          }
          str += " ";

          for (int i = 0; i < 16; i++)
          {
               char ch = (i < iCount) ? Convert.ToChar(abyBuffer[i]) : ' ';
               str += Char.IsControl(ch) ? "." : ch.ToString();
          }
          return str;
     }
}