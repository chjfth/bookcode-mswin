//----------------------------------------
// MultiCopy.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class MultiCopy: Form
{
     const string strFormat = "MultiCopy.InternalFormat";

     float[,] afValues = {{ 0.12f, 3.45f, 6.78f, 9.01f },
                          { 2.34f, 5.67f, 8.90f, 1.23f },
                          { 4.56f, 7.89f, 0.12f, 3.45f }};
     
     public static void Main()
     {
          Application.Run(new MultiCopy());
     }
     public MultiCopy()
     {
          Text = "Multi Copy";
          Menu = new MainMenu();

               // Edit menu

          MenuItem mi = new MenuItem("&Edit");
          Menu.MenuItems.Add(mi);

               // Edit Copy menu item

          mi = new MenuItem("&Copy");
          mi.Click += new EventHandler(MenuEditCopyOnClick);
          mi.Shortcut = Shortcut.CtrlC;
          Menu.MenuItems[0].MenuItems.Add(mi);
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          DataObject data = new DataObject();

               // Define internal clipboard format.

          MemoryStream memorystream = new MemoryStream();
          BinaryWriter binarywriter = new BinaryWriter(memorystream);

          binarywriter.Write(afValues.GetLength(0));
          binarywriter.Write(afValues.GetLength(1));

          for (int iRow = 0; iRow < afValues.GetLength(0); iRow++)
          for (int iCol = 0; iCol < afValues.GetLength(1); iCol++)
               binarywriter.Write(afValues[iRow, iCol]);
          
          binarywriter.Close();

          data.SetData(strFormat, memorystream);

               // Define CSV clipboard format.

          memorystream = new MemoryStream();
          StreamWriter streamwriter = new StreamWriter(memorystream);

          for (int iRow = 0; iRow < afValues.GetLength(0); iRow++)
          for (int iCol = 0; iCol < afValues.GetLength(1); iCol++)
          {
               streamwriter.Write(afValues[iRow, iCol]);

               if (iCol < afValues.GetLength(1) - 1)
                    streamwriter.Write(",");
               else
                    streamwriter.WriteLine();
          }
          streamwriter.Write("\0");
          streamwriter.Close();
          data.SetData(DataFormats.CommaSeparatedValue, memorystream);

               // Define String clipboard format.

          StringWriter stringwriter = new StringWriter();

          for (int iRow = 0; iRow < afValues.GetLength(0); iRow++)
          for (int iCol = 0; iCol < afValues.GetLength(1); iCol++)
          {
               stringwriter.Write(afValues[iRow, iCol]);

               if (iCol < afValues.GetLength(1) - 1)
                    stringwriter.Write("\t");
               else
                    stringwriter.WriteLine();
          }
          stringwriter.Close();
          data.SetData(stringwriter.ToString());

          Clipboard.SetDataObject(data, false);
     }
}
