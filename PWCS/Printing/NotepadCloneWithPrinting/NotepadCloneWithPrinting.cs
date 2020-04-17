//-------------------------------------------------------
// NotepadCloneWithPrinting.cs © 2001 by Charles Petzold
//-------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.IO;
using System.Windows.Forms;

class NotepadCloneWithPrinting: NotepadCloneWithFormat
{
     PrintDocument      prndoc = new PrintDocument();
     PageSetupDialog    setdlg = new PageSetupDialog();
     PrintPreviewDialog predlg = new PrintPreviewDialog();
     PrintDialog        prndlg = new PrintDialog();
     string             strPrintText;
     int                iStartPage, iNumPages, iPageNumber;

     public new static void Main()
     {
		  System.Threading.Thread.CurrentThread.ApartmentState =
			                            System.Threading.ApartmentState.STA;
		 
          Application.Run(new NotepadCloneWithPrinting());
     }
     public NotepadCloneWithPrinting()
     {
          strProgName = "Notepad Clone with Printing";
          MakeCaption();

          prndoc.PrintPage += new PrintPageEventHandler(OnPrintPage);
          setdlg.Document = prndoc;
          predlg.Document = prndoc;
          prndlg.Document = prndoc;

          prndlg.AllowSomePages = true;
          prndlg.PrinterSettings.FromPage = 1;
          prndlg.PrinterSettings.ToPage = 
                                   prndlg.PrinterSettings.MaximumPage;
     }
     protected override void MenuFileSetupOnClick(object obj, EventArgs ea)
     {
          setdlg.ShowDialog();
     }
     protected override void MenuFilePreviewOnClick(object obj, EventArgs ea)
     {
          prndoc.DocumentName = Text;   // Just in case it's printed

          strPrintText = txtbox.Text;
          iStartPage   = 1;
          iNumPages    = prndlg.PrinterSettings.MaximumPage;
          iPageNumber  = 1;

          predlg.ShowDialog();
     }
     protected override void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
          prndlg.AllowSelection = txtbox.SelectionLength > 0;

          if (prndlg.ShowDialog() == DialogResult.OK)
          {
               prndoc.DocumentName = Text;

                    // Initialize some important fields.

               switch (prndlg.PrinterSettings.PrintRange)
               {
               case PrintRange.AllPages:
                    strPrintText = txtbox.Text;
                    iStartPage   = 1;
                    iNumPages    = prndlg.PrinterSettings.MaximumPage;
                    break;

               case PrintRange.Selection:
                    strPrintText = txtbox.SelectedText;
                    iStartPage   = 1;
                    iNumPages    = prndlg.PrinterSettings.MaximumPage;
                    break;

               case PrintRange.SomePages:
                    strPrintText = txtbox.Text;
                    iStartPage   = prndlg.PrinterSettings.FromPage;
                    iNumPages    = prndlg.PrinterSettings.ToPage - 
                                        iStartPage + 1;
                    break;
               }
                    // And commence printing.

               iPageNumber  = 1;
               prndoc.Print();
          }
     }
     void OnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics     grfx   = ppea.Graphics;
          Font         font   = txtbox.Font;
          float        cyFont = font.GetHeight(grfx);
          StringFormat strfmt = new StringFormat();
          RectangleF   rectfFull, rectfText;
          int          iChars, iLines;

               // Calculate RectangleF for header and footer.

          if (grfx.VisibleClipBounds.X < 0)       // Print preview
               rectfFull = ppea.MarginBounds;
          else                                    // Regular print
               rectfFull = new RectangleF(
                    ppea.MarginBounds.Left - (ppea.PageBounds.Width - 
                              grfx.VisibleClipBounds.Width) / 2,
                    ppea.MarginBounds.Top - (ppea.PageBounds.Height - 
                              grfx.VisibleClipBounds.Height) / 2,
                    ppea.MarginBounds.Width, ppea.MarginBounds.Height);

               // Calculate RectangleF for text.

          rectfText = RectangleF.Inflate(rectfFull, 0, -2 * cyFont);

          int iDisplayLines = (int) Math.Floor(rectfText.Height / cyFont);
          rectfText.Height = iDisplayLines * cyFont;

               // Set up StringFormat object for rectanglar display of text.

          if (txtbox.WordWrap)
          {
               strfmt.Trimming = StringTrimming.Word;
          }
          else
          {
               strfmt.Trimming = StringTrimming.EllipsisCharacter;
               strfmt.FormatFlags |= StringFormatFlags.NoWrap;
          }
               // For "some pages" get to the first page.

          while ((iPageNumber < iStartPage) && (strPrintText.Length > 0))
          {
               if (txtbox.WordWrap)
                    grfx.MeasureString(strPrintText, font, rectfText.Size, 
                                       strfmt, out iChars, out iLines);
               else
                    iChars = CharsInLines(strPrintText, iDisplayLines);

               strPrintText = strPrintText.Substring(iChars);
               iPageNumber++;
          }
               // If we've prematurely run out of text, cancel the print job.

          if (strPrintText.Length == 0)
          {
               ppea.Cancel = true;
               return;
          }
               // Display text for this page

          grfx.DrawString(strPrintText, font, Brushes.Black, 
                          rectfText, strfmt);

               // Get text for next page.

          if (txtbox.WordWrap)
               grfx.MeasureString(strPrintText, font, rectfText.Size, 
                                  strfmt, out iChars, out iLines);
          else
               iChars = CharsInLines(strPrintText, iDisplayLines);

          strPrintText = strPrintText.Substring(iChars);

               // Reset StringFormat display header and footer.
          
          strfmt = new StringFormat();

               // Display filename at top.

          strfmt.Alignment = StringAlignment.Center;
          grfx.DrawString(FileTitle(), font, Brushes.Black, 
                          rectfFull, strfmt);

               // Display page number at bottom.

          strfmt.LineAlignment = StringAlignment.Far;
          grfx.DrawString("Page " + iPageNumber, font, Brushes.Black, 
                          rectfFull, strfmt);

               // Decide whether to print another page.

          iPageNumber++;
          ppea.HasMorePages = (strPrintText.Length > 0) && 
                              (iPageNumber < iStartPage + iNumPages);

               // Reinitialize variables for printing from preview form.

          if (!ppea.HasMorePages)
          {
               strPrintText = txtbox.Text;
               iStartPage   = 1;
               iNumPages    = prndlg.PrinterSettings.MaximumPage;
               iPageNumber  = 1;
          }
     }
     int CharsInLines(string strPrintText, int iNumLines)
     {
          int index = 0;

          for (int i = 0; i < iNumLines; i++)
          {
               index = 1 + strPrintText.IndexOf('\n', index);

               if (index == 0)
                    return strPrintText.Length;
          }
          return index;
     }
}