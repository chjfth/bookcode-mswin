//----------------------------------------
// LineJoins.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class LineJoins: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new LineJoins());
     }
     public LineJoins()
     {
          Text = "Line Joins: Miter, Bevel, Round, MiterClipped";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen     penNarrow = new Pen(clr);
          Pen     penWide   = new Pen(Color.Gray, cx / 16);
          Point[] apt       = { new Point(1 * cx / 32, 1 * cy / 8), 
                                new Point(4 * cx / 32, 6 * cy / 8),
                                new Point(7 * cx / 32, 1 * cy / 8) };

          for (int i = 0; i < 4; i++)
          {
               penWide.LineJoin = (LineJoin) i;

               grfx.DrawLines(penWide, apt);
               grfx.DrawLines(penNarrow, apt);
               grfx.TranslateTransform(cx / 4, 0);
          }
     }
}