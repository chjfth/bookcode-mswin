//----------------------------------------------
// WildCardHexDump.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.IO;

class WildCardHexDump: HexDump
{
     public new static int Main(string[] astrArgs)
     {
          if (astrArgs.Length == 0)
          {
               Console.WriteLine("Syntax: WildCardHexDump file1 file2 ...");
               return 1;
          }
          foreach (string str in astrArgs)
               ExpandWildCard(str);

          return 0;
     }
     static void ExpandWildCard(string strWildCard)
     {
          string[] astrFiles; 

          try
          {
               astrFiles = Directory.GetFiles(strWildCard);
          }
          catch 
          {
               try
               {
                    string strDir  = Path.GetDirectoryName(strWildCard);
                    string strFile = Path.GetFileName(strWildCard);

                    if (strDir == null || strDir.Length == 0)
                         strDir = ".";

                    astrFiles = Directory.GetFiles(strDir, strFile);
               }
               catch
               {
                    Console.WriteLine(strWildCard + ": No Files found!");
                    return;
               }
          }
          if (astrFiles.Length == 0)
               Console.WriteLine(strWildCard + ": No files found!");

          foreach(string strFile in astrFiles)
               DumpFile(strFile);
     }
}
