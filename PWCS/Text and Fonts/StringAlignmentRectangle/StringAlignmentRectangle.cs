//-------------------------------------------------------
// StringAlignmentRectangle.cs © 2001 by Charles Petzold
//-------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class StringAlignmentRectangle: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new StringAlignmentRectangle());
     }
     public StringAlignmentRectangle()
     {
          Text = "String Alignment (RectangleF in DrawString)";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush    = new SolidBrush(clr);
          RectangleF   rectf    = new RectangleF(0, 0, cx, cy);
          string[]     strAlign = { "Near", "Center", "Far" };
          StringFormat strfmt   = new StringFormat();

          for (int iVert = 0; iVert < 3; iVert++)
          for (int iHorz = 0; iHorz < 3; iHorz++)
          {
               strfmt.LineAlignment = (StringAlignment)iVert;
               strfmt.Alignment     = (StringAlignment)iHorz;

               grfx.DrawString(
                    String.Format("LineAlignment = {0}\nAlignment = {1}",
                                  strAlign[iVert], strAlign[iHorz]),
                    Font, brush, rectf, strfmt);
          }
     }
}
