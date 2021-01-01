//----------------------------------------------
// AntiAliasedText.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;

class AntiAliasedText: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new AntiAliasedText());
     }
     public AntiAliasedText()
     {
          Text = "Anti-Aliased Text" ;
          Font = new Font("Times New Roman", 12);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush  brush  = new SolidBrush(clr);
          string str    = "A ";
          int    cxText = (int) grfx.MeasureString(str, Font).Width;

          for (int i = 0; i < 6; i++)
          {
               grfx.TextRenderingHint = (TextRenderingHint)i;
               grfx.DrawString(str, Font, brush, i * cxText, 0);
          }
     }
}
