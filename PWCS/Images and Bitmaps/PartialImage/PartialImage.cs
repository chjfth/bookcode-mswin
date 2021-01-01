//-------------------------------------------
// PartialImage.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PartialImage: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new PartialImage());
     }
     public PartialImage()
     {
          Text = "Partial Image";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Rectangle rect = new Rectangle(95, 5, 50, 55);

          grfx.DrawImage(image, 0, 0, rect, GraphicsUnit.Pixel);
     }
}
