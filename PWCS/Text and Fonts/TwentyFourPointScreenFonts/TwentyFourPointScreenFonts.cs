//---------------------------------------------------------
// TwentyFourPointScreenFonts.cs (R) 2001 by Charles Petzold
//---------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwentyFourPointScreenFonts: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TwentyFourPointScreenFonts());
     }
     public TwentyFourPointScreenFonts()
     {
          Text = "Twenty-Four Point Screen Fonts";
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

          font = new Font(strFamily, grfx.DpiY / 3, GraphicsUnit.Pixel);
          grfx.DrawString("GraphicsUnit.Pixel, " + grfx.DpiY / 3 + " units",
                          font, brush, 0, y);
          y += font.GetHeight(grfx);

          font = new Font(strFamily, grfx.DpiY / 3, GraphicsUnit.World);
          grfx.DrawString("GraphicsUnit.World, " + grfx.DpiY / 3 + " units", 
                          font, brush, 0, y);
     }
}
