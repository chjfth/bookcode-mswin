//---------------------------------------------
// TextOnBaseline.cs (R) 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TextOnBaseline: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TextOnBaseline());
     }
     public TextOnBaseline()
     {
          Text = "Text on Baseline";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          float yBaseline = cy / 2;
          Pen   pen       = new Pen(clr);

               // Draw the baseline across the center of the client area.

          grfx.DrawLine(pen, 0, yBaseline, cx, yBaseline);

               // Create a 144-point font.

          Font font = new Font("Times New Roman", 144);

               // Get and calculate some metrics.

          float cyLineSpace = font.GetHeight(grfx);
          int   iCellSpace  = font.FontFamily.GetLineSpacing(font.Style);
          int   iCellAscent = font.FontFamily.GetCellAscent(font.Style);
          float cyAscent    = cyLineSpace * iCellAscent / iCellSpace;

               // Display the text on the baseline.

          grfx.DrawString("Baseline", font, new SolidBrush(clr),
                          0, yBaseline - cyAscent);
     }
}
