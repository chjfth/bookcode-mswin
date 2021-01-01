//------------------------------------------------
// MetafilePageUnits.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Printing;     // Not used for anything yet!
using System.Windows.Forms;

class MetafilePageUnits: PrintableForm
{
     Metafile mf;

     public new static void Main()
     {
          Application.Run(new MetafilePageUnits());
     }
     public MetafilePageUnits()
     {
          Text = "Metafile Page Units";

               // Create metafile.

          Graphics grfx  = CreateGraphics();
          IntPtr ipHdc = grfx.GetHdc();

          mf = new Metafile("MetafilePageUnits.emf", ipHdc);

          grfx.ReleaseHdc(ipHdc);
          grfx.Dispose();

               // Get Graphics object for drawing on metafile.

          grfx = Graphics.FromImage(mf);
          grfx.Clear(Color.White);

               // Draw in units of pixels (1-point pen width).

          grfx.PageUnit = GraphicsUnit.Pixel;
          Pen pen = new Pen(Color.Black, grfx.DpiX / 72);
          grfx.DrawRectangle(pen, 0, 0, grfx.DpiX, grfx.DpiY);

               // Draw in units of 1/100 inch (1-point pen width).

          grfx.PageUnit = GraphicsUnit.Inch;
          grfx.PageScale = 0.01f;
          pen = new Pen(Color.Black, 100f / 72);
          grfx.DrawRectangle(pen, 25, 25, 100, 100);

               // Draw in units of millimeters (1-point pen width).

          grfx.PageUnit = GraphicsUnit.Millimeter;
          grfx.PageScale = 1;
          pen = new Pen(Color.Black, 25.4f / 72);
          grfx.DrawRectangle(pen, 12.7f, 12.7f, 25.4f, 25.4f);

               // Draw in units of points (1-point pen width).

          grfx.PageUnit = GraphicsUnit.Point;
          pen = new Pen(Color.Black, 1);
          grfx.DrawRectangle(pen, 54, 54, 72, 72);

          grfx.Dispose();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawImage(mf, 0, 0);
     }
}
