//-------------------------------------------------
// DropShadowWithPath.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class DropShadowWithPath: FontMenuForm
{
     const int iOffset = 10;   // Approximately 1/10 inch (exactly on printer)

     public new static void Main()
     {
          Application.Run(new DropShadowWithPath());
     }
     public DropShadowWithPath()
     {
          Text = "Drop Shadow with Path";
          Width *= 2;
          strText = "Shadow";
          font = new Font("Times New Roman", 108);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          float fFontSize = PointsToPageUnits(grfx, font);
               
               // Get coordinates for a centered string.

          SizeF sizef = grfx.MeasureString(strText, font);
          PointF ptf = new PointF((cx - sizef.Width) / 2, 
                                  (cy - sizef.Height) / 2);
               
               // Add the text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, ptf, new StringFormat());

               // Clear, fill, translate, fill, and draw.

          grfx.Clear(Color.White);
          grfx.FillPath(Brushes.Black, path);
          path.Transform(new Matrix(1, 0, 0, 1, -10, -10));
          grfx.FillPath(Brushes.White, path);
          grfx.DrawPath(Pens.Black, path);
     }
}
