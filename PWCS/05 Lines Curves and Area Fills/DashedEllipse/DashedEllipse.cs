//--------------------------------------------
// DashedEllipse.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DashedEllipse: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new DashedEllipse());
     }
     public DashedEllipse()
     {
          Text = "Dashed Ellipse Using DrawArc";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen       pen  = new Pen(clr);
          Rectangle rect = new Rectangle(0, 0, cx - 1, cy - 1);

          for (int iAngle = 0; iAngle < 360; iAngle += 15)
               grfx.DrawArc(pen, rect, iAngle, 10);
     }
}
