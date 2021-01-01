//----------------------------------------------------------
// PrintableTenCentimeterRuler.cs (R) 2001 by Charles Petzold
//----------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PrintableTenCentimeterRuler: TenCentimeterRuler
{
     public new static void Main()
     {
          Application.Run(new PrintableTenCentimeterRuler());
     }
     public PrintableTenCentimeterRuler()
     {
          Text = "Printable " + Text;
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.PageUnit = GraphicsUnit.Pixel;
          base.DoPage(grfx, clr, cx, cy);
     }
}
