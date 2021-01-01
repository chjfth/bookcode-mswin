//---------------------------------------
// WrapText.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class WrapText: FontMenuForm
{
     float fRadius = 100;

     public new static void Main()
     {
          Application.Run(new WrapText());
     }
     public WrapText()
     {
          Text = "Wrap Text";

          strText = "e snake ate the tail of th";
          font = new Font("Times New Roman", 48);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          float fFontSize = PointsToPageUnits(grfx, font);

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, new PointF(0, 0), new StringFormat());

               // Shift the origin to left baseline, y increasing up.

          RectangleF rectf = path.GetBounds();
          path.Transform(new Matrix(1, 0, 0, -1, -rectf.Left, 
                                                 GetAscent(grfx, font)));
               // Scale so width equals 2*PI.
 
          float fScale = 2 * (float) Math.PI / rectf.Width;
          path.Transform(new Matrix(fScale, 0, 0, fScale, 0, 0));
 
               // Modify the path.

          PointF[] aptf  = path.PathPoints;

          for (int i = 0; i < aptf.Length; i++)
               aptf[i] = new PointF(
                    fRadius * (1 + aptf[i].Y) * (float) Math.Cos(aptf[i].X),
                    fRadius * (1 + aptf[i].Y) * (float) Math.Sin(aptf[i].X));

          path = new GraphicsPath(aptf, path.PathTypes);

               // Fill the path.

          grfx.TranslateTransform(cx / 2, cy / 2);
          grfx.FillPath(new SolidBrush(clr), path);
     }
}