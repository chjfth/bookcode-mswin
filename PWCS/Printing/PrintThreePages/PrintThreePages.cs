//----------------------------------------------
// PrintThreePages.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class PrintThreePages: Form
{
     const int iNumberPages = 3;
     int       iPageNumber;

     public static void Main()
     {
          Application.Run(new PrintThreePages());
     }
     public PrintThreePages()
     {
          Text = "Print Three Pages";

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

               // Set printer resolution to "draft".

          foreach (PrinterResolution prnres in 
                                   prndoc.PrinterSettings.PrinterResolutions)
          {
               if (prnres.Kind == PrinterResolutionKind.Draft)
                    prndoc.DefaultPageSettings.PrinterResolution = prnres;
          }

               // Set remainder of PrintDocument properties.

          prndoc.DocumentName = Text;
          prndoc.PrintPage += new PrintPageEventHandler(OnPrintPage);
          prndoc.QueryPageSettings += new QueryPageSettingsEventHandler
                                             (OnQueryPageSettings);
               // Commence printing.

          iPageNumber = 1;
          prndoc.Print();
     }
     void OnQueryPageSettings(object obj, QueryPageSettingsEventArgs qpsea)
     {
          if (qpsea.PageSettings.PrinterSettings.LandscapeAngle != 0)
               qpsea.PageSettings.Landscape ^= true;
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

          ppea.HasMorePages = iPageNumber < iNumberPages;
          iPageNumber += 1;
     }
}
