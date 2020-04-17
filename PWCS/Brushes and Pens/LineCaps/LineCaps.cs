//---------------------------------------
// LineCaps.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class LineCaps: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new LineCaps());
     }
     public LineCaps()
     {
          Text = "Line Caps";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen   penWide   = new Pen(Color.Gray, Font.Height);
          Pen   penNarrow = new Pen(clr);
          Brush brush     = new SolidBrush(clr);

          foreach (LineCap lc in Enum.GetValues(typeof(LineCap)))
          {
               grfx.DrawString(lc.ToString(), Font, brush, 
                               Font.Height, Font.Height / 2);

               penWide.StartCap = lc;
               penWide.EndCap   = lc;

               grfx.DrawLine(penWide, 2 * cx / 4, Font.Height, 
                                      3 * cx / 4, Font.Height); 

               grfx.DrawLine(penNarrow, 2 * cx / 4, Font.Height, 
                                        3 * cx / 4, Font.Height); 

               grfx.TranslateTransform(0, 2 * Font.Height);
          }
     }
}
