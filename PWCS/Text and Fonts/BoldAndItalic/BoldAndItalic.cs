//--------------------------------------------
// BoldAndItalic.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BoldAndItalic: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new BoldAndItalic());
     }
     public BoldAndItalic()
     {
          Text = "Bold and Italic Text";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          const string str1        = "This is some ";
          const string str2        = "bold";
          const string str3        = " text and this is some ";
          const string str4        = "italic";
          const string str5        = " text.";
          Brush        brush       = new SolidBrush(clr);
          Font         fontRegular = Font;
          Font         fontBold    = new Font(fontRegular, FontStyle.Bold);
          Font         fontItalic  = new Font(fontRegular, FontStyle.Italic);
          float        x           = 0;
          float        y           = 0;

          grfx.DrawString(str1, fontRegular, brush, x, y);
          x += grfx.MeasureString(str1, fontRegular).Width;

          grfx.DrawString(str2, fontBold, brush, x, y);
          x += grfx.MeasureString(str2, fontBold).Width;

          grfx.DrawString(str3, fontRegular, brush, x, y);
          x += grfx.MeasureString(str3, fontRegular).Width;

          grfx.DrawString(str4, fontItalic, brush, x, y);
          x += grfx.MeasureString(str4, fontItalic).Width;

          grfx.DrawString(str5, fontRegular, brush, x, y);
     }
}
