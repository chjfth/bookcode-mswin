//------------------------------------------
// SimpleShear.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class SimpleShear: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new SimpleShear());
     }
     public SimpleShear()
     {
          Text = "Simple Shear";

          strText = "Shear";
          font = new Font("Times New Roman", 72);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush  brush = new SolidBrush(clr);
          Matrix matx  = new Matrix();

          matx.Shear(0.5f, 0);
          grfx.Transform = matx;

          grfx.DrawString(strText, font, brush, 0, 0);
     }
}