//------------------------------------------------
// HollowFontWidePen.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HollowFontWidePen: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new HollowFontWidePen());
     }
     public HollowFontWidePen()
     {
          Text = "Hollow Font (Wide Pen)";
          Width *= 2; 
          strText = "Wide Pen";
          font = new Font("Times New Roman", 108, 
                          FontStyle.Bold | FontStyle.Italic);
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

          Brush brush = new HatchBrush(HatchStyle.Trellis, 
                                       Color.White, Color.Black);
          Pen pen = new Pen(brush, fFontSize / 20);
          grfx.DrawPath(pen, path);
     }
}