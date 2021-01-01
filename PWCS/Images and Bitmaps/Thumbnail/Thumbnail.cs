//----------------------------------------
// Thumbnail.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class Thumbnail: PrintableForm
{
     const int iSquare = 64;
     Image imageThumbnail;

     public new static void Main()
     {
          Application.Run(new Thumbnail());
     }
     public Thumbnail()
     {
          Text = "Thumbnail";

          Image image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");

          int cxThumbnail, cyThumbnail;

          if (image.Width > image.Height)
          {
               cxThumbnail = iSquare;
               cyThumbnail = iSquare * image.Height / image.Width;
          }
          else
          {
               cyThumbnail = iSquare;
               cxThumbnail = iSquare * image.Width / image.Height;
          }
          imageThumbnail = image.GetThumbnailImage(cxThumbnail, cyThumbnail,
                                                   null, (IntPtr) 0);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          for (int y = 0; y < cy; y += iSquare)
          for (int x = 0; x < cx; x += iSquare)
               grfx.DrawImage(imageThumbnail, 
                              x + (iSquare - imageThumbnail.Width) / 2,
                              y + (iSquare - imageThumbnail.Height) / 2,
							  imageThumbnail.Width, imageThumbnail.Height);
     }
} 