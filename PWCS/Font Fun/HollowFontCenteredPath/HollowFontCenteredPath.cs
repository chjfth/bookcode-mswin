//-----------------------------------------------------
// HollowFontCenteredPath.cs (R) 2001 by Charles Petzold
//-----------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HollowFontCenteredPath: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new HollowFontCenteredPath());
     }
     public HollowFontCenteredPath()
     {
          Text = "Hollow Font (Centered Path)";
          Width *= 2; 
          strText = "Hollow";
          font = new Font("Times New Roman", 108);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          float fFontSize = PointsToPageUnits(grfx, font);

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, new PointF(0, 0), new StringFormat());

               // Get the path bounds for centering.

          RectangleF rectfBounds = path.GetBounds();
          
          grfx.TranslateTransform(
                         (cx - rectfBounds.Width) / 2 - rectfBounds.Left,
                         (cy - rectfBounds.Height) / 2 - rectfBounds.Top);

               // Draw the path.

          Pen pen = new Pen(clr, fFontSize / 50);
          pen.DashStyle = DashStyle.Dot;

          grfx.DrawPath(pen, path);
     }
}