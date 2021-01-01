//----------------------------------------------
// TallInTheCenter.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TallInTheCenter: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new TallInTheCenter());
     }
     public TallInTheCenter()
     {
          Text = "Tall in the Center";
          Width *= 2;
          strText = Text;
          font = new Font("Times New Roman", 48);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();
          float fFontSize = PointsToPageUnits(grfx, font);

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, new PointF(0, 0), new StringFormat());

               // Shift the origin to the center of the path.

          RectangleF rectf = path.GetBounds();

          path.Transform(new Matrix(1, 0, 0, 1, 
                                    -(rectf.Left + rectf.Right) / 2,
                                    -(rectf.Top + rectf.Bottom) / 2));
          rectf = path.GetBounds();

               // Modify the path.

          PointF[] aptf = path.PathPoints;

          for (int i = 0; i < aptf.Length; i++)
               aptf[i].Y *= 2 * (rectf.Width - Math.Abs(aptf[i].X)) / 
                                                                 rectf.Width; 
          path = new GraphicsPath(aptf, path.PathTypes);

               // Fill the path.

          grfx.TranslateTransform(cx / 2, cy / 2);
          grfx.FillPath(new SolidBrush(clr), path);
     }
}