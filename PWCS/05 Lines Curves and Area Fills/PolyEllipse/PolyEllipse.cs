//------------------------------------------
// PolyEllipse.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PolyEllipse: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new PolyEllipse());
     }
     public PolyEllipse()
     {
          Text = "Ellipse with DrawLines";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          int      iNum = 2 * (cx + cy);
          PointF[] aptf = new PointF[iNum];

          for (int i = 0; i < iNum; i++)
          {
               double dAng = i * 2 * Math.PI / (iNum - 1);

               aptf[i].X = (cx - 1) / 2f * (1 + (float)Math.Cos(dAng));
               aptf[i].Y = (cy - 1) / 2f * (1 + (float)Math.Sin(dAng));
          }
          grfx.DrawLines(new Pen(clr), aptf);
     }
}
