//----------------------------------------------------------
// TwoPointLinearGradientBrush.cs (R) 2001 by Charles Petzold
//----------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TwoPointLinearGradientBrush: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TwoPointLinearGradientBrush());
     }
     TwoPointLinearGradientBrush()
     {
          Text = "Two-Point Linear Gradient Brush";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          LinearGradientBrush lgbrush = 
               new LinearGradientBrush(
                         new Point(cx / 4, cy / 4), 
                         new Point(3 * cx / 4, 3 * cy / 4),
                         Color.White, Color.Black);

          grfx.FillRectangle(lgbrush, 0, 0, cx, cy);
     }
}
