//-------------------------------------------
// SixInchRuler.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SixInchRuler: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new SixInchRuler());
     }
     public SixInchRuler()
     {
          Text = "Six-Inch Ruler";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush     brush   = new SolidBrush(clr);
          Pen       pen     = new Pen(clr, 0.5f);
          const int xOffset = 16;
          const int yOffset = 16;

          grfx.PageUnit  = GraphicsUnit.Inch;
          grfx.PageScale = 1 / 64f;
          grfx.DrawRectangle(pen, xOffset, yOffset, 6 * 64, 64);

          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = StringAlignment.Center;

          for (int i = 1; i < 6 * 16; i++)
          {
               int x = xOffset + i * 4;
               int y = yOffset;
               int dy;

               if (i % 16 == 0)         // One-inch marks
               {
                    dy = 32;
                    grfx.DrawString((i / 16).ToString(), Font, brush,
                                                  x, y + dy, strfmt);
               }
               else if (i % 8 == 0)     // Half-inch marks
                    dy = 24;

               else if (i % 4 == 0)     // Quarter-inch marks
                    dy = 20;

               else if (i % 2 == 0)     // Eighth-inch marks
                    dy = 16;

               else                     // Sixteeenth-inch marks
                    dy = 12;

               grfx.DrawLine(pen, x, y, x, y + dy);
          }
     }
}
