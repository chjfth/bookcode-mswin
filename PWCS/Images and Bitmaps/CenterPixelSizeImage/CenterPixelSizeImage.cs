//---------------------------------------------------
// CenterPixelSizeImage.cs © 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CenterPixelSizeImage: PrintableForm
{
	Image image;

	public new static void Main()
	{
		Application.Run(new CenterPixelSizeImage());
	}
	public CenterPixelSizeImage()
	{
		Text = "Center Pixel-Size Image";

		image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
	}
	protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
	{
		grfx.DrawImage(image, (cx - image.Width)  / 2,
			(cy - image.Height) / 2,
			image.Width, image.Height);
	}
}
