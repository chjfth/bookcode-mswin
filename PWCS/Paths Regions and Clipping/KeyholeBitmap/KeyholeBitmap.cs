//--------------------------------------------
// KeyholeBitmap.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Windows.Forms;

class KeyholeBitmap: PrintableForm
{
     Bitmap bitmap;

     public new static void Main()
     {
          Application.Run(new KeyholeBitmap());
     }
     public KeyholeBitmap()
     {
          Text = "Keyhole Bitmap";

               // Load image.
          
          Image image = Image.FromFile(
               "..\\..\\..\\..\\Images and Bitmaps\\Apollo11FullColor.jpg");

               // Create clipping path.

          GraphicsPath path = new GraphicsPath();
          path.AddArc(80, 0, 80, 80, 45, -270);
          path.AddLine(70, 180, 170, 180);

               // Get size of clipping path.

          RectangleF rectf = path.GetBounds();
 
               // Create new bitmap initialized to transparent.

          bitmap = new Bitmap((int) rectf.Width, (int) rectf.Height, 
                              PixelFormat.Format32bppArgb);

               // Create Graphics object based on new bitmap.

          Graphics grfx = Graphics.FromImage(bitmap);

               // Draw original image on bitmap with clipping.

          grfx.SetClip(path);
          grfx.TranslateClip(-rectf.X, -rectf.Y);
          grfx.DrawImage(image, (int) -rectf.X, (int) -rectf.Y,
                                 image.Width, image.Height);
          grfx.Dispose();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawImage(bitmap, (cx - bitmap.Width) / 2, 
                                 (cy - bitmap.Height) / 2,
                                 bitmap.Width, bitmap.Height);
     }
}
