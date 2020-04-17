//------------------------------------------
// LineArcPath.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class LineArcPath: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new LineArcPath());
     }
     public LineArcPath()
     {
          Text = "Line & Arc in Path";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          Pen          pen  = new Pen(clr, 25);

          path.AddLine( 25, 100, 125, 100);
          path.AddArc (125,  50, 100, 100, -180, 180);
          path.AddLine(225, 100, 325, 100);

          grfx.DrawPath(pen, path);
     }
}
