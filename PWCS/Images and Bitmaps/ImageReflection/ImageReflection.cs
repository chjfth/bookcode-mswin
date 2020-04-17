//----------------------------------------------
// ImageReflection.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ImageReflection: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new ImageReflection());
     }
     public ImageReflection()
     {
          Text = "Image Reflection";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          int cxImage = image.Width;
          int cyImage = image.Height;

          grfx.DrawImage(image, cx / 2, cy / 2,  cxImage,  cyImage);
          grfx.DrawImage(image, cx / 2, cy / 2, -cxImage,  cyImage);
          grfx.DrawImage(image, cx / 2, cy / 2,  cxImage, -cyImage);
          grfx.DrawImage(image, cx / 2, cy / 2, -cxImage, -cyImage);
     }
}
