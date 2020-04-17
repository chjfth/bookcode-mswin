//-------------------------------------------------
// BetterFamiliesList.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterFamiliesList: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new BetterFamiliesList());
     }
     public BetterFamiliesList()
     {
          Text = "Better Font Families List";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush = new SolidBrush(clr);
          float        y     = 0;
          FontFamily[] aff   = FontFamily.Families;

          foreach (FontFamily ff in aff)
          {
               if (ff.IsStyleAvailable(FontStyle.Regular))
               {
                    Font font = new Font(ff, 12);
                    grfx.DrawString(ff.Name, font, brush, 0, y);
                    y += font.GetHeight(grfx);
               }
               else
               {
                    grfx.DrawString("* " + ff.Name, Font, brush, 0, y);
                    y += Font.GetHeight(grfx);
               }
          }
     }
}
