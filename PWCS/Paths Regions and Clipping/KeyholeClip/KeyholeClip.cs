//------------------------------------------
// KeyholeClip.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class KeyholeClip: PrintableForm
{
     protected Image        image;
     protected GraphicsPath path;

     public new static void Main()
     {
          Application.Run(new KeyholeClip());
     }
     public KeyholeClip()
     {
          Text = "Keyhole Clip";
          
          image = Image.FromFile(
               "..\\..\\..\\..\\Images and Bitmaps\\Apollo11FullColor.jpg");

          path = new GraphicsPath();
          path.AddArc(80, 0, 80, 80, 45, -270);
          path.AddLine(70, 180, 170, 180);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.SetClip(path);
          grfx.DrawImage(image, 0, 0, image.Width, image.Height);
     }
}
