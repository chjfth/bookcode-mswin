//----------------------------------------------
// BoxingTheClient.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BoxingTheClient: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new BoxingTheClient());
     }
     public BoxingTheClient()
     {
          Text = "Boxing the Client";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Point[] apt = {new Point(0,      0),
                         new Point(cx - 1, 0),
                         new Point(cx - 1, cy - 1),
                         new Point(0,      cy - 1),
                         new Point(0,      0)};

          grfx.DrawLines(new Pen(clr), apt);
     }
}
