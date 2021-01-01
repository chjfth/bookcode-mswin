//----------------------------------------
// SineCurve.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SineCurve: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new SineCurve());
     }
     public SineCurve()
     {
          Text = "Sine Curve";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          PointF[] aptf = new PointF[cx];

          for (int i = 0; i < cx; i++)
          {
               aptf[i].X = i;
               aptf[i].Y = cy / 2 * (1 - (float) 
                                   Math.Sin(i * 2 * Math.PI / (cx - 1)));
          }
          grfx.DrawLines(new Pen(clr), aptf);
     }
}
