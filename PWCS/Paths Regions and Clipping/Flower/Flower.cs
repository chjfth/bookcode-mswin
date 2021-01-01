//-------------------------------------
// Flower.cs (R) 2001 by Charles Petzold
//-------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class Flower: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new Flower());
     }
     public Flower()
     {
          Text = "Flower";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {   
               // Draw green stem from lower left corner to center.

          grfx.DrawBezier(new Pen(Color.Green, 10), 
                    new Point(0, cy),          new Point(0, 3 * cy / 4),
                    new Point(cx / 4, cy / 4), new Point(cx / 2, cy / 2));

               // Set up transform for remainder of flower.

          float fScale = Math.Min(cx, cy) / 2000f;
          grfx.TranslateTransform(cx / 2, cy / 2);
          grfx.ScaleTransform(fScale, fScale);

               // Draw red petals.

          GraphicsPath path = new GraphicsPath();

          path.AddBezier(new Point(  0,    0), new Point(125,  125),
                         new Point(475,  125), new Point(600,    0));
          path.AddBezier(new Point(600,    0), new Point(475, -125), 
                         new Point(125, -125), new Point(  0,    0));

          for (int i = 0; i < 8; i++)
          {
               grfx.FillPath(Brushes.Red, path);
               grfx.DrawPath(Pens.Black, path);
               grfx.RotateTransform(360 / 8);
          }
               // Draw yellow circle in center.

          Rectangle rect = new Rectangle(-150, -150, 300, 300);
          grfx.FillEllipse(Brushes.Yellow, rect);
          grfx.DrawEllipse(Pens.Black, rect);
     }
}