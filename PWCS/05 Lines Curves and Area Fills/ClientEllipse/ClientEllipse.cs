//--------------------------------------------
// ClientEllipse.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ClientEllipse: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new ClientEllipse());
     }
     public ClientEllipse()
     {
          Text = "Client Ellipse";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawEllipse(new Pen(clr), 0, 0, cx - 1, cy - 1);
     }
}
