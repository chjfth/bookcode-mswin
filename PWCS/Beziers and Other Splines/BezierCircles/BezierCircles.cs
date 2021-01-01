//--------------------------------------------
// BezierCircles.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BezierCircles: PrintableForm
{
     public new static void Main()
     { 
          Application.Run(new BezierCircles());
     }
     public BezierCircles()
     {
          Text = "Bezier Circles";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          int iRadius = Math.Min(cx - 1, cy - 1) / 2;

          grfx.DrawEllipse(new Pen(clr), cx / 2 - iRadius, cy / 2 - iRadius, 
                                         2 * iRadius, 2 * iRadius);

               // Two-segment (180-degree) approximation

          int L = (int) Math.Round(iRadius * 4f / 3 * Math.Tan(Math.PI / 4));

          Point[] apt = {
                              new Point(cx / 2,     cy / 2 - iRadius),
                              new Point(cx / 2 + L, cy / 2 - iRadius),
                              new Point(cx / 2 + L, cy / 2 + iRadius), 
                              new Point(cx / 2,     cy / 2 + iRadius),
                              new Point(cx / 2 - L, cy / 2 + iRadius),
                              new Point(cx / 2 - L, cy / 2 - iRadius),
                              new Point(cx / 2,     cy / 2 - iRadius)
                        };
          grfx.DrawBeziers(Pens.Blue, apt);

               // Four-segment (90-degree) approximation

          L = (int) Math.Round(iRadius * 4f / 3 * Math.Tan(Math.PI / 8));

          apt = new Point[] 
                         {
                              new Point(cx / 2,           cy / 2 - iRadius),
                              new Point(cx / 2 + L,       cy / 2 - iRadius),
                              new Point(cx / 2 + iRadius, cy / 2 - L),
                              new Point(cx / 2 + iRadius, cy / 2),
                              new Point(cx / 2 + iRadius, cy / 2 + L),
                              new Point(cx / 2 + L,       cy / 2 + iRadius), 
                              new Point(cx / 2,           cy / 2 + iRadius),
                              new Point(cx / 2 - L,       cy / 2 + iRadius),
                              new Point(cx / 2 - iRadius, cy / 2 + L),
                              new Point(cx / 2 - iRadius, cy / 2),
                              new Point(cx / 2 - iRadius, cy / 2 - L),
                              new Point(cx / 2 - L,       cy / 2 - iRadius),
                              new Point(cx / 2,           cy / 2 - iRadius)
                         };
          grfx.DrawBeziers(Pens.Red, apt);
     }
}
