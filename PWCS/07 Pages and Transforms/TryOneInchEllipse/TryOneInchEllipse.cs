//------------------------------------------------
// TryOneInchEllipse.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TryOneInchEllipse: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TryOneInchEllipse());
     }
     public TryOneInchEllipse()
     {
          Text = "Try One-Inch Ellipse";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawEllipse(new Pen(clr), 0, 0, grfx.DpiX, grfx.DpiY);
     }
}
