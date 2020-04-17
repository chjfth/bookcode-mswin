//---------------------------------------
// ClipText.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class ClipText: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new ClipText());
     }
     public ClipText()
     {
          Text = "Clip Text";
          Width *= 2; 
          strText = "Clip Text";
          font = new Font("Times New Roman", 108, FontStyle.Bold);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          float fFontSize = PointsToPageUnits(grfx, font);

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, new PointF(0, 0), new StringFormat());

               // Set the clipping region.

          grfx.SetClip(path);

               // Get the path bounds and center the clipping region.

          RectangleF rectfBounds = path.GetBounds();
          
          grfx.TranslateClip(
                         (cx - rectfBounds.Width) / 2 - rectfBounds.Left,
                         (cy - rectfBounds.Height) / 2 - rectfBounds.Top);

               // Draw clipped lines.

          Random rand = new Random();

          for (int y = 0; y < cy; y++)
          {
               Pen pen = new Pen(Color.FromArgb(rand.Next(255),
                                                rand.Next(255),
                                                rand.Next(255)));

               grfx.DrawBezier(pen, new Point(0, y),
                                    new Point(cx / 3, y + cy / 3),
                                    new Point(2 * cx / 3, y - cy / 3),
                                    new Point(cx, y));
          }
     }
}
