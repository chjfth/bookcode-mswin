//---------------------------------------
// Infinity.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class Infinity: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new Infinity());
     }
     public Infinity()
     {
          Text = "Infinity Sign Using Bezier Splines";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          cx--;
          cy--;

          Point[] apt = 
          {
               new Point(0,          cy / 2),     // Begin
               new Point(0,          0),          // Control
               new Point(    cx / 3, 0),          // Control
               new Point(    cx / 2, cy / 2),     // End/Begin
               new Point(2 * cx / 3, cy),         // Control
               new Point(    cx,     cy),         // Control
               new Point(    cx,     cy / 2),     // End/Begin
               new Point(    cx,     0),          // Control
               new Point(2 * cx / 3, 0),          // Control
               new Point(    cx / 2, cy / 2),     // End/Begin
               new Point(    cx / 3, cy),         // Control
               new Point(0,          cy),         // Control
               new Point(0,          cy / 2)      // End
          };
          grfx.DrawBeziers(new Pen(clr), apt);
     }
}
