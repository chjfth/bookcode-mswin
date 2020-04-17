//-----------------------------------------
// HollowFont.cs © 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HollowFont: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new HollowFont());
     }
     public HollowFont()
     {
          Text = "Hollow Font";
          Width *= 2; 
          strText = "Hollow";
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

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         fFontSize, ptf, new StringFormat());

               // Draw the path.

          grfx.DrawPath(new Pen(clr), path);
     }
}