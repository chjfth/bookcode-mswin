//-----------------------------------------------
// HelloWorldBitmap.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HelloWorldBitmap: PrintableForm
{
     const  float fResolution = 300;
     Bitmap bitmap;

     public new static void Main()
     {
          Application.Run(new HelloWorldBitmap());
     }
     public HelloWorldBitmap()
     {
          Text = "Hello, World!";

          bitmap = new Bitmap(1, 1);
          bitmap.SetResolution(fResolution, fResolution);

          Graphics grfx = Graphics.FromImage(bitmap);
          Font     font = new Font("Times New Roman", 72);
          Size     size = grfx.MeasureString(Text, font).ToSize();

          bitmap = new Bitmap(bitmap, size);
          bitmap.SetResolution(fResolution, fResolution);
               
          grfx = Graphics.FromImage(bitmap);
          grfx.Clear(Color.White);
          grfx.DrawString(Text, font, Brushes.Black, 0, 0);
          grfx.Dispose();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawImage(bitmap, 0, 0);
     }
}
