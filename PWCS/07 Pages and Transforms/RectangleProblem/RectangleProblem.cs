//-----------------------------------------------
// RectangleProblem.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RectangleProblem: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new RectangleProblem());
     }
     public RectangleProblem()
     {
          Text = "Rectangle Problem";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen pen = new Pen(clr, 0.1f);

          grfx.PageUnit  = GraphicsUnit.Inch;
          grfx.PageScale = 0.1f;

          grfx.DrawRectangle(pen,  0,  0, 10, 10);
          grfx.DrawRectangle(pen, 10, 10, 10, 10);
     }
}
