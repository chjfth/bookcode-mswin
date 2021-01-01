//----------------------------------------
// BezierArt.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BezierArt: PrintableForm
{
     const int iNum = 100;

     public new static void Main()
     {
          Application.Run(new BezierArt());
     }
     public BezierArt()
     {
          Text = "Bezier Art";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen      pen  = new Pen(clr);
          PointF[] aptf = new PointF[4];

          for (int i = 0; i < iNum; i++)
          {
               double dAngle = 2 * i * Math.PI / iNum;

               aptf[0].X =     cx / 2 + cx /  2 * (float) Math.Cos(dAngle);
               aptf[0].Y = 5 * cy / 8 + cy / 16 * (float) Math.Sin(dAngle);

               aptf[1] = new PointF(cx / 2,    -cy);
               aptf[2] = new PointF(cx / 2, 2 * cy);

               dAngle += Math.PI;

               aptf[3].X = cx / 2 + cx /  4 * (float) Math.Cos(dAngle);
               aptf[3].Y = cy / 2 + cy / 16 * (float) Math.Sin(dAngle);

               grfx.DrawBeziers(pen, aptf);
          }
     }
}
