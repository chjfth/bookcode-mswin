//------------------------------------------------
// NaiveFamiliesList.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class NaiveFamiliesList: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new NaiveFamiliesList());
     }
     public NaiveFamiliesList()
     {
          Text = "Naive Font Families List";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush = new SolidBrush(clr);
          float        y     = 0;
          FontFamily[] aff   = FontFamily.Families;

          foreach (FontFamily ff in aff)
          {
               Font font = new Font(ff, 12);
               grfx.DrawString(ff.Name, font, brush, 0, y);
               y += font.GetHeight(grfx);
          }
     }
}
