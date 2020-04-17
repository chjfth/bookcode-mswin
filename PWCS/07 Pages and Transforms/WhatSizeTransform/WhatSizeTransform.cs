//------------------------------------------------
// WhatSizeTransform.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class WhatSizeTransform: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new WhatSizeTransform());
     }
     public WhatSizeTransform()
     {
          Text = "What Size? With TransformPoints";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush   brush = new SolidBrush(clr);
          int     y     = 0;
          Point[] apt   = { new Point(cx, cy) };

          grfx.TransformPoints(CoordinateSpace.Device, 
                               CoordinateSpace.Page, apt);

          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Pixel);
          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Display);
          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Document);
          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Inch);
          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Millimeter);
          DoIt(grfx, brush, ref y, apt[0], GraphicsUnit.Point);
     }
     void DoIt(Graphics grfx, Brush brush, ref int y, 
               Point pt, GraphicsUnit gu)
     {
          GraphicsState gs = grfx.Save();

          grfx.PageUnit  = gu;
          grfx.PageScale = 1;

          PointF[] aptf = { pt };

          grfx.TransformPoints(CoordinateSpace.Page, 
                               CoordinateSpace.Device, aptf);

          SizeF sizef = new SizeF(aptf[0]);
          grfx.Restore(gs);

          grfx.DrawString(gu + ": " + sizef, Font, brush, 0, y);
          y += (int) Math.Ceiling(Font.GetHeight(grfx));
     }
}
