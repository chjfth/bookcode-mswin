//-----------------------------------------------
// PrintWithMargins.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class PrintWithMargins: Form
{
     public static void Main()
     {
          Application.Run(new PrintWithMargins());
     }
     public PrintWithMargins()
     {
          Text = "Print with Margins";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&File");
          Menu.MenuItems[0].MenuItems.Add("&Print...", 
                                   new EventHandler(MenuFilePrintOnClick));
     }
     void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
               // Create PrintDocument.

          PrintDocument prndoc = new PrintDocument();

               // Create dialog box and set PrinterName property.

          PrinterSelectionDialog dlg = new PrinterSelectionDialog();
          dlg.PrinterName = prndoc.PrinterSettings.PrinterName;

               // Show dialog box and bail out if not OK.

          if (dlg.ShowDialog() != DialogResult.OK)
               return;

               // Set PrintDocument to selected printer.

          prndoc.PrinterSettings.PrinterName = dlg.PrinterName;

               // Set remainder of PrintDocument properties and commence.

          prndoc.DocumentName = Text;
          prndoc.PrintPage += new PrintPageEventHandler(OnPrintPage);
          prndoc.Print();
     }
     void OnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics   grfx  = ppea.Graphics;
          RectangleF rectf = new RectangleF(
               ppea.MarginBounds.Left - 
               (ppea.PageBounds.Width - grfx.VisibleClipBounds.Width) / 2,
               ppea.MarginBounds.Top - 
               (ppea.PageBounds.Height - grfx.VisibleClipBounds.Height) / 2,
               ppea.MarginBounds.Width,
               ppea.MarginBounds.Height);

          grfx.DrawRectangle(Pens.Black, rectf.X, rectf.Y, 
                                         rectf.Width, rectf.Height);

          grfx.DrawLine(Pens.Black, rectf.Left, rectf.Top, 
                                    rectf.Right, rectf.Bottom);

          grfx.DrawLine(Pens.Black, rectf.Right, rectf.Top, 
                                    rectf.Left, rectf.Bottom);
     }
}
