//--------------------------------------------------
// ImageScaleIsotropic.cs (R) 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ImageScaleIsotropic: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new ImageScaleIsotropic());
     }
     public ImageScaleIsotropic()
     {
          Text = "Image Scale Isotropic";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          ScaleImageIsotropically(grfx, image, new Rectangle(0, 0, cx, cy));
     }
     void ScaleImageIsotropically(Graphics grfx, Image image, Rectangle rect)
     {
          SizeF sizef = new SizeF(image.Width / image.HorizontalResolution,
                                  image.Height / image.VerticalResolution);

          float fScale = Math.Min(rect.Width  / sizef.Width,
                                  rect.Height / sizef.Height);

          sizef.Width  *= fScale;
          sizef.Height *= fScale;
          
          grfx.DrawImage(image, rect.X + (rect.Width  - sizef.Width ) / 2,
                                rect.Y + (rect.Height - sizef.Height) / 2,
                                sizef.Width, sizef.Height);
     }
}

