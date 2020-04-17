//----------------------------------------
// DrawHouse.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DrawHouse: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new DrawHouse());
     }
     public DrawHouse()
     {
          Text = "Draw a House in One Line";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawLines(new Pen(clr),
                         new Point[] 
                         {
                         new Point(    cx / 4, 3 * cy / 4), // Lower left
                         new Point(    cx / 4,     cy / 2),
                         new Point(    cx / 2,     cy / 4), // Peak
                         new Point(3 * cx / 4,     cy / 2),
                         new Point(3 * cx / 4, 3 * cy / 4), // Lower right
                         new Point(    cx / 4,     cy / 2),
                         new Point(3 * cx / 4,     cy / 2),
                         new Point(    cx / 4, 3 * cy / 4), // Lower left
                         new Point(3 * cx / 4, 3 * cy / 4)  // Lower right
                         });
     }
}
