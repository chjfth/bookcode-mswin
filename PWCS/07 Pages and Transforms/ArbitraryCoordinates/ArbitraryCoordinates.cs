// --------------------------------------------------
// ArbitraryCoordinates.cs © 2001 by Charles Petzold
// --------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ArbitraryCoordinates: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new ArbitraryCoordinates());
     }
     public ArbitraryCoordinates()
     {
          Text = "Arbitrary Coordinates";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.PageUnit = GraphicsUnit.Pixel;
          SizeF sizef = grfx.VisibleClipBounds.Size;

          grfx.PageUnit  = GraphicsUnit.Inch;
          grfx.PageScale = Math.Min(sizef.Width  / grfx.DpiX / 1000, 
                                    sizef.Height / grfx.DpiY / 1000);

          grfx.DrawEllipse(new Pen(clr), 0, 0, 990, 990);
     }
}
