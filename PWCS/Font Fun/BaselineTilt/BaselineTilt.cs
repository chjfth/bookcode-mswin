//-------------------------------------------
// BaselineTilt.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class BaselineTilt: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new BaselineTilt());
     }
     public BaselineTilt()
     {
          Text = "Baseline Tilt";

          strText = "Baseline";
          font = new Font("Times New Roman", 144);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          float yBaseline = 3 * cy / 4;
          float cyAscent = GetAscent(grfx, font);

          grfx.DrawLine(new Pen(clr), 0, yBaseline, cx, yBaseline);

          grfx.TranslateTransform(0, yBaseline);

          Matrix matx = grfx.Transform;
          matx.Shear(-0.5f, 0);
          grfx.Transform = matx;

          grfx.DrawString(strText, font, new SolidBrush(clr), 0, -cyAscent);
     }
}