//-------------------------------------------------
// PartialImageRotate.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PartialImageRotate: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new PartialImageRotate());
     }
     public PartialImageRotate()
     {
          Text = "Partial Image Rotate";

          image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Point[] aptDst = { new Point(0, cy / 2),
                             new Point(cx / 2, 0),
                             new Point(cx / 2, cy) };

          Rectangle rectSrc = new Rectangle(95, 5, 50, 55);
          
          grfx.DrawImage(image, aptDst, rectSrc, GraphicsUnit.Pixel);
     }
}
