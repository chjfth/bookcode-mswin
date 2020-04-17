//--------------------------------------------------
// PartialImageStretch.cs © 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PartialImageStretch: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new PartialImageStretch());
     }
     public PartialImageStretch()
     {
          Text = "Partial Image Stretch";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Rectangle rectSrc = new Rectangle(95, 5, 50, 55);
          Rectangle rectDst = new Rectangle( 0, 0, cx, cy);

          grfx.DrawImage(image, rectDst, rectSrc, GraphicsUnit.Pixel);
     }
}
