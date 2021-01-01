//-------------------------------------
// Clover.cs (R) 2001 by Charles Petzold
//-------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class Clover: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new Clover());
     }
     public Clover()
     {
          Text = "Clover";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();

          path.AddEllipse(0,      cy / 3, cx / 2, cy / 3);  // Left
          path.AddEllipse(cx / 2, cy / 3, cx / 2, cy / 3);  // Right
          path.AddEllipse(cx / 3, 0,      cx / 3, cy / 2);  // Top
          path.AddEllipse(cx / 3, cy / 2, cx / 3, cy / 2);  // Bottom

          grfx.SetClip(path);
          grfx.TranslateTransform(cx / 2, cy / 2);

          Pen   pen     = new Pen(clr);
          float fRadius = (float) Math.Sqrt(Math.Pow(cx / 2, 2) + 
                                            Math.Pow(cy / 2, 2));
     
          for (float fAngle = 0; fAngle <  (float) Math.PI * 2; 
                                 fAngle += (float) Math.PI / 180)
          {
               grfx.DrawLine(pen, 0, 0, fRadius * (float) Math.Cos(fAngle),
                                       -fRadius * (float) Math.Sin(fAngle));
          }
     }
}