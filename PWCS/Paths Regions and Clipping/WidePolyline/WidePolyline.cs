//-------------------------------------------
// WidePolyline.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class WidePolyline: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new WidePolyline());
     }
     public WidePolyline()
     {
          Text = "Wide Polyline";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen pen = new Pen(clr, 25);

          grfx.DrawLines(pen, new Point[] {
                         new Point( 25, 100), new Point(125, 100),
                         new Point(125,  50), new Point(225,  50),
                         new Point(225, 100), new Point(325, 100) });
     }
}
