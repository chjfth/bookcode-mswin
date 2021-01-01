//-----------------------------------------
// FullFit.cs (R) 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class FullFit: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new FullFit());
     }
     public FullFit()
     {
          Text = "Full Fit";

          strText = "Full Fit";
          font = new Font("Times New Roman", 108);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
               
               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         100, new Point(0, 0), new StringFormat());

               // Set the world transform.

          RectangleF rectfBounds = path.GetBounds();
          PointF[]   aptfDest    = { new PointF(0, 0), new PointF(cx, 0),
                                                       new PointF(0, cy) };
          
          grfx.Transform = new Matrix(rectfBounds, aptfDest);

               // Fill the path.

          grfx.FillPath(new SolidBrush(clr), path);
     }
}