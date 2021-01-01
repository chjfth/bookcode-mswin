//---------------------------------------
// HtmlDump.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.IO;
using System.Net;

class HtmlDump
{
     public static int Main(string[] astrArgs)
     {
          if (astrArgs.Length == 0)
          {
               Console.WriteLine("Syntax: HtmlDump URI");
               return 1;
          }

          WebRequest webreq;
          WebResponse webres;

          try
          {
               webreq = WebRequest.Create(astrArgs[0]);
               webres = webreq.GetResponse();
          }
          catch (Exception exc)
          {
               Console.WriteLine("HtmlDump: {0}", exc.Message);
               return 1;
          }

          if (webres.ContentType.Substring(0, 4) != "text")
          {
               Console.WriteLine("HtmlDump: URI must be a text type.");
               return 1;
          }

          Stream       stream = webres.GetResponseStream();
          StreamReader strrdr = new StreamReader(stream);
          string       strLine;
          int          iLine = 1;

          while ((strLine = strrdr.ReadLine()) != null)
               Console.WriteLine("{0:D5}: {1}", iLine++, strLine);

          stream.Close();
          return 0;
     }
}
