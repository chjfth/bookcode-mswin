//-------------------------------------------
// BezierManual.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BezierManual: Bezier
{
     public new static void Main()
     {
          Application.Run(new BezierManual());
     }
     public BezierManual()
     {
          Text = "Bezier Curve \"Manually\" Drawn";
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          base.OnPaint(pea);

          BezierSpline(pea.Graphics, Pens.Red, apt);
     }
     void BezierSpline(Graphics grfx, Pen pen, Point[] aptDefine)
     {
          Point[] apt = new Point[100];

          for (int i = 0; i < apt.Length; i++)
          {
               float t = (float) i / (apt.Length - 1);

               float x = (1 - t) * (1 - t) * (1 - t) * aptDefine[0].X +
                          3 * t  * (1 - t) * (1 - t) * aptDefine[1].X +
                          3 * t * t        * (1 - t) * aptDefine[2].X +
                          t * t * t                  * aptDefine[3].X;

               float y = (1 - t) * (1 - t) * (1 - t) * aptDefine[0].Y +
                          3 * t  * (1 - t) * (1 - t) * aptDefine[1].Y +
                          3 * t * t        * (1 - t) * aptDefine[2].Y +
                          t * t * t                  * aptDefine[3].Y;

               apt[i] = new Point((int) Math.Round(x), (int) Math.Round(y));
          }
          grfx.DrawLines(pen, apt);
     }
}
