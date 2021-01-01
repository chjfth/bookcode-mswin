//------------------------------------------------
// StarGradientBrush.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class StarGradientBrush: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new StarGradientBrush());
     }
     public StarGradientBrush()
     {
          Text = "Star Gradient Brush";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Point[] apt = new Point[5];
          
          for (int i = 0; i < apt.Length; i++)
          {
               double dAngle = (i * 0.8 - 0.5) * Math.PI;
               apt[i] = new Point(
                         (int)(cx * (0.50 + 0.48 * Math.Cos(dAngle))),
                         (int)(cy * (0.50 + 0.48 * Math.Sin(dAngle))));
          }
          PathGradientBrush pgbrush = new PathGradientBrush(apt);

          pgbrush.CenterColor = Color.White;
          pgbrush.SurroundColors = new Color[1] { Color.Black };

          grfx.FillRectangle(pgbrush, 0, 0, cx, cy);
    }
}
