//------------------------------------------
// GradientPen.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class GradientPen: PrintableForm 
{
     public new static void Main()
     {
          Application.Run(new GradientPen());
     }
     public GradientPen()
     {
          Text = "Gradient Pen";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush lgbrush = new LinearGradientBrush(
                                   new Rectangle(0, 0, cx, cy), 
                                   Color.White, Color.Black,
                                   LinearGradientMode.BackwardDiagonal);

          Pen pen = new Pen(lgbrush, Math.Min(cx, cy) / 25);

          pen.Alignment = PenAlignment.Inset;

          grfx.DrawRectangle(pen, 0, 0, cx, cy);
          grfx.DrawLine(pen, 0, 0, cx, cy);
          grfx.DrawLine(pen, 0, cy, cx, 0);
     }
}
