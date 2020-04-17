//----------------------------------------------------------
// TwentyFourPointPrinterFonts.cs © 2001 by Charles Petzold
//----------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwentyFourPointPrinterFonts: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TwentyFourPointPrinterFonts());
     }
     public TwentyFourPointPrinterFonts()
     {
          Text = "Twenty-Four Point Printer Fonts";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush  brush     = new SolidBrush(clr);
          float  y         = 0;
          Font   font;
          string strFamily = "Times New Roman";

          font = new Font(strFamily, 24);
          grfx.DrawString("No GraphicsUnit, 24 points", font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 24, GraphicsUnit.Point);
          grfx.DrawString("GraphicsUnit.Point, 24 units", font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 1/ 3f, GraphicsUnit.Inch);
          grfx.DrawString("GraphicsUnit.Inch, 1/3 units", font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 25.4f / 3, GraphicsUnit.Millimeter);
          grfx.DrawString("GraphicsUnit.Millimeter, 25.4/3 units", 
                          font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 100, GraphicsUnit.Document);
          grfx.DrawString("GraphicsUnit.Document, 100 units", 
                          font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 100f / 3, GraphicsUnit.Pixel);
          grfx.DrawString("GraphicsUnit.Pixel, " + 100f / 3 + " units",
                          font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, 100f / 3, GraphicsUnit.World);
          grfx.DrawString("GraphicsUnit.World, " + 100f / 3 + " units", 
                          font, brush, 0, y);
     }
}
