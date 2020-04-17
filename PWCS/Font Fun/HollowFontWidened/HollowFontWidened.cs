//------------------------------------------------
// HollowFontWidened.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HollowFontWidened: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new HollowFontWidened());
     }
     public HollowFontWidened()
     {
          Text = "Hollow Font (Widened)";
          Width *= 2; 
          strText = "Widened";
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

               // Widen, fill, and draw the path.

          path.Widen(new Pen(Color.Black, fFontSize / 20));
          Brush brush = new HatchBrush(HatchStyle.Trellis, 
                                       Color.White, Color.Black);
          grfx.DrawPath(new Pen(Color.Black, 2), path);
          grfx.FillPath(brush, path);
     }
}