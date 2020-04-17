//------------------------------------------
// DotsPerInch.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DotsPerInch: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new DotsPerInch());
     }
     public DotsPerInch()
     {
          Text = "Dots Per Inch";   
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawString(String.Format("DpiX = {0}\nDpiY = {1}",
                                        grfx.DpiX, grfx.DpiY),
                          Font, new SolidBrush(clr), 0, 0);
     }
}
