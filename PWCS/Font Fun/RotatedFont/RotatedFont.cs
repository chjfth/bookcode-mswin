//------------------------------------------
// RotatedFont.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RotatedFont: FontMenuForm
{
     const int iDegrees = 20;      // Should be divisor of 360

     public new static void Main()
     {
          Application.Run(new RotatedFont());
     }
     public RotatedFont()
     {
          Text = "Rotated Font";

          strText = "   Rotated Font";
          font = new Font("Arial", 18);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush brush = new SolidBrush(clr);
          StringFormat strfmt = new StringFormat();
          strfmt.LineAlignment = StringAlignment.Center;
          
          grfx.TranslateTransform(cx / 2, cy / 2);

          for (int i = 0; i < 360; i += iDegrees)
          {
               grfx.DrawString(strText, font, brush, 0, 0, strfmt);
               grfx.RotateTransform(iDegrees);
          }
     }
}