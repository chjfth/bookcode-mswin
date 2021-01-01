//-------------------------------------------------
// StringWriterAppend.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.IO;

class StringWriterAppend
{
	const int iIterations = 10000;

	public static void Main()
	{
		DateTime     dt = DateTime.Now;
		StringWriter sw = new StringWriter();

		for (int i = 0; i < iIterations; i++)
			sw.WriteLine("abcdefghijklmnopqrstuvwxyz");

		string str = sw.ToString();

		Console.WriteLine(DateTime.Now - dt);
	}
}
