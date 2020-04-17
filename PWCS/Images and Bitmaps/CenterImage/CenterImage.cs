//------------------------------------------
// CenterImage.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CenterImage: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new CenterImage());
     }
     public CenterImage()
     {
          Text = "Center Image";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.PageUnit = GraphicsUnit.Pixel;
          grfx.PageScale = 1;

          RectangleF rectf = grfx.VisibleClipBounds;

          float cxImage = grfx.DpiX * image.Width / 
                                             image.HorizontalResolution;
          float cyImage = grfx.DpiY * image.Height / 
                                             image.VerticalResolution;

          grfx.DrawImage(image, (rectf.Width  - cxImage) / 2, 
                                (rectf.Height - cyImage) / 2);
     }
}
