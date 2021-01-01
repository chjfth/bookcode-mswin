//-------------------------------------------------
// TenCentimeterRuler.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TenCentimeterRuler: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TenCentimeterRuler());
     }
     public TenCentimeterRuler()
     {
          Text = "Ten-Centimeter Ruler";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen       pen   = new Pen(clr);
          Brush     brush = new SolidBrush(clr);
          const int xOffset = 10;
          const int yOffset = 10;

          grfx.DrawPolygon(pen, new PointF[] 
               { 
                    MMConv(grfx, new PointF(xOffset,       yOffset)),
                    MMConv(grfx, new PointF(xOffset + 100, yOffset)),
                    MMConv(grfx, new PointF(xOffset + 100, yOffset + 10)),
                    MMConv(grfx, new PointF(xOffset,       yOffset + 10))
               });

          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = StringAlignment.Center;

          for (int i = 1; i < 100; i++)
          {
               if (i % 10 == 0)         // Centimeter markings
               {
                    grfx.DrawLine(pen, 
                         MMConv(grfx, new PointF(xOffset + i, yOffset)),
                         MMConv(grfx, new PointF(xOffset + i, yOffset + 5)));

                    grfx.DrawString((i/10).ToString(), Font, brush, 
                         MMConv(grfx, new PointF(xOffset + i, yOffset + 5)), 
                         strfmt);
               }
               else if (i % 5 == 0)     // Half-centimeter markings
               {
                    grfx.DrawLine(pen, 
                         MMConv(grfx, new PointF(xOffset + i, yOffset)),
                         MMConv(grfx, new PointF(xOffset + i, yOffset + 3)));
               }
               else                     // Millimeter markings
               {
                    grfx.DrawLine(pen, 
                      MMConv(grfx, new PointF(xOffset + i, yOffset)),
                      MMConv(grfx, new PointF(xOffset + i, yOffset + 2.5f)));
               }
          }
     }
     PointF MMConv(Graphics grfx, PointF pointf)
     {
	     pointf.X *= grfx.DpiX / 25.4f;
	     pointf.Y *= grfx.DpiY / 25.4f;

	     return pointf;
     }
}
