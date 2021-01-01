//-------------------------------------------
// LineArcCombo.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class LineArcCombo: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new LineArcCombo());
     }
     public LineArcCombo()
     {
          Text = "Line & Arc Combo";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen pen = new Pen(clr, 25);

          grfx.DrawLine(pen,  25, 100, 125, 100);
          grfx.DrawArc (pen, 125,  50, 100, 100, -180, 180);
          grfx.DrawLine(pen, 225, 100, 325, 100);
     }
}
