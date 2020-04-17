//---------------------------------------------------
// CreateMetafileMemory.cs © 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

class CreateMetafileMemory: PrintableForm
{
     readonly MemoryStream ms = new MemoryStream();

     public new static void Main()
     {
          Application.Run(new CreateMetafileMemory());
     }
     public CreateMetafileMemory()
     {
          Text = "Create Metafile (Memory)";

               // Create the metafile.

          Graphics grfx = CreateGraphics();
          IntPtr ipHdc = grfx.GetHdc();

          Metafile mf = new Metafile(ms, ipHdc); 

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
          ms.Position = 0;
          Metafile mf = new Metafile(ms);

          for (int y = 0; y < cy; y += mf.Height)
          for (int x = 0; x < cx; x += mf.Width)
               grfx.DrawImage(mf, x, y, mf.Width, mf.Height);
     }
}
