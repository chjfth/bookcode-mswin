//------------------------------------------------
// SimplePrintDialog.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class PrintDialogHelloWorld: Form
{
     const int iNumberPages = 3;
     int       iPagesToPrint, iPageNumber;

     public static void Main()
     {
          Application.Run(new PrintDialogHelloWorld());
     }
     public PrintDialogHelloWorld()
     {
          Text = "Simple PrintDialog";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&File");
          Menu.MenuItems[0].MenuItems.Add("&Print...", 
                                   new EventHandler(MenuFilePrintOnClick));
     }
     void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
               // Create the PrintDocument and PrintDialog.

          PrintDocument prndoc = new PrintDocument();
          PrintDialog   prndlg = new PrintDialog();
          prndlg.Document = prndoc;

               // Allow a page range.

          prndlg.AllowSomePages = true;
          prndlg.PrinterSettings.MinimumPage = 1;
          prndlg.PrinterSettings.MaximumPage = iNumberPages;
          prndlg.PrinterSettings.FromPage = 1;
          prndlg.PrinterSettings.ToPage = iNumberPages;

               // If the dialog box returns OK, print.

          if(prndlg.ShowDialog() == DialogResult.OK)
          {
               prndoc.DocumentName = Text;
               prndoc.PrintPage += new PrintPageEventHandler(OnPrintPage);

                    // Determine which pages to print.

               switch (prndlg.PrinterSettings.PrintRange)
               {
               case PrintRange.AllPages:
                    iPagesToPrint = iNumberPages;
                    iPageNumber = 1;
                    break;

               case PrintRange.SomePages:
                    iPagesToPrint = 1 + prndlg.PrinterSettings.ToPage -
                                        prndlg.PrinterSettings.FromPage;
                    iPageNumber = prndlg.PrinterSettings.FromPage;
                    break;
               }
               prndoc.Print();
          }
     }
     void OnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics grfx  = ppea.Graphics;
          Font     font  = new Font("Times New Roman", 360);
          string   str   = iPageNumber.ToString();
          SizeF    sizef = grfx.MeasureString(str, font);

          grfx.DrawString(str, font, Brushes.Black, 
                    (grfx.VisibleClipBounds.Width - sizef.Width) / 2,
                    (grfx.VisibleClipBounds.Height - sizef.Height) / 2);

          iPageNumber += 1;
          iPagesToPrint -= 1;
          ppea.HasMorePages = iPagesToPrint > 0;
     }
}
