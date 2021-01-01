//--------------------------------------------------
// HundredPixelsSquare.cs (R) 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HundredPixelsSquare: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new HundredPixelsSquare());
     }
     public HundredPixelsSquare()
     {
          Text = "Hundred Pixels Square";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.FillRectangle(new SolidBrush(clr), 100, 100, 100, 100);
     }
}
