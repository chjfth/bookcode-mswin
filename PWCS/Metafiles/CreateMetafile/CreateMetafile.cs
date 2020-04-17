//---------------------------------------------
// CreateMetafile.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;              // Not used for anything yet!
using System.Windows.Forms;

class CreateMetafile: PrintableForm
{
     Metafile mf;

     public new static void Main()
     {
          Application.Run(new CreateMetafile());
     }
     public CreateMetafile()
     {
          Text = "Create Metafile";

               // Create the metafile.

          Graphics grfx  = CreateGraphics();
          IntPtr ipHdc = grfx.GetHdc();

          mf = new Metafile("CreateMetafile.emf", ipHdc);

          grfx.ReleaseHdc(ipHdc);
          grfx.Dispose();

               // Draw on the metafile.

          grfx = Graphics.FromImage(mf);

          grfx.FillEllipse(Brushes.Gray, 0, 0, 100, 100);
          grfx.DrawEllipse(Pens.Black, 0, 0, 100, 100);
          grfx.FillEllipse(Brushes.Blue, 20, 20, 20, 20);
          grfx.FillEllipse(Brushes.Blue, 60, 20, 20, 20);
          grfx.DrawArc(new Pen(Color.Red, 10), 20, 20, 60, 60, 30, 120);
          grfx.Dispose();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          for (int y = 0; y < cy; y += mf.Height)
          for (int x = 0; x < cx; x += mf.Width)
               grfx.DrawImage(mf, x, y, mf.Width, mf.Height);
     }
}
