//------------------------------------------------
// MyDocumentsFolder.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Windows.Forms;

class MyDocumentsFolder
{
     public static void Main()
     {
          MessageBox.Show(
               Environment.GetFolderPath(Environment.SpecialFolder.Personal),
               "My Documents Folder");
     }
}