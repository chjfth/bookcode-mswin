//----------------------------------------
// FontNames.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FontNames: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new FontNames());
     }
     public FontNames()
     {
          Text = "Font Names";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {    
          string[]    astrFonts = { "Courier New", "Arial", 
                                    "Times New Roman" };
          FontStyle[] afs       = { FontStyle.Regular, FontStyle.Bold, 
                                    FontStyle.Italic,  
                                    FontStyle.Bold | FontStyle.Italic };
          Brush       brush     = new SolidBrush(clr);
          float       y         = 0;

          foreach (string strFont in astrFonts)
          {
               foreach (FontStyle fs in afs)
               {
                    Font font = new Font(strFont, 18, fs);
                    grfx.DrawString(strFont, font, brush, 0, y);
                    y += font.GetHeight(grfx);
               }
          }
     }
}
