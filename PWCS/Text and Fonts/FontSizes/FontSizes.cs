//----------------------------------------
// FontSizes.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FontSizes: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new FontSizes());
     }
     public FontSizes()
     {
          Text = "Font Sizes";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {    
          string strFont = "Times New Roman";
          Brush  brush   = new SolidBrush(clr);
          float  y       = 0;

          for (float fSize = 6; fSize <= 12; fSize += 0.25f)
          {
               Font font = new Font(strFont, fSize);
               grfx.DrawString(strFont + " in " + fSize + " points", 
                               font, brush, 0, y);
               y += font.GetHeight(grfx);
          }
     }
}
