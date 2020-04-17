//------------------------------------------
// DrawOnImage.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DrawOnImage: PrintableForm
{
     Image  image;
     string str = "Apollo11";

     public new static void Main()
     {
          Application.Run(new DrawOnImage());
     }
     public DrawOnImage()
     {
          Text  = "Draw on Image";
          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");

          Graphics grfxImage = Graphics.FromImage(image);

          grfxImage.PageUnit = GraphicsUnit.Inch;
          grfxImage.PageScale = 1;

          SizeF sizef = grfxImage.MeasureString(str, Font);

          grfxImage.DrawString(str, Font, Brushes.White, 
                         grfxImage.VisibleClipBounds.Width - sizef.Width, 0);

          grfxImage.Dispose();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
		  grfx.PageUnit = GraphicsUnit.Pixel;
          grfx.DrawImage(image, 0, 0);
          grfx.DrawString(str, Font, new SolidBrush(clr),
                    grfx.DpiX * image.Width / image.HorizontalResolution, 0);
     }
}