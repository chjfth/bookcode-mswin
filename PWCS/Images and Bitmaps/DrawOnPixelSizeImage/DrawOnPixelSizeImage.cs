//---------------------------------------------------
// DrawOnPixelSizeImage.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DrawOnPixelSizeImage: PrintableForm
{
	Image  image;
	string str = "Apollo11";

	public new static void Main()
	{
		Application.Run(new DrawOnPixelSizeImage());
	}
	public DrawOnPixelSizeImage()
	{
		Text  = "Draw on Pixel-Size Image";
		image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");

		Graphics grfxImage  = Graphics.FromImage(image);
		Graphics grfxScreen = CreateGraphics();

		Font font = new Font(Font.FontFamily, 
			grfxScreen.DpiY / grfxImage.DpiY * Font.SizeInPoints);

		SizeF sizef = grfxImage.MeasureString(str, font);

		grfxImage.DrawString(str, font, Brushes.White, 
			image.Width - sizef.Width, 0);
		grfxImage.Dispose();
		grfxScreen.Dispose();
	}
	protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
	{
		grfx.DrawImage(image, 0, 0, image.Width, image.Height);
		grfx.DrawString(str, Font, new SolidBrush(clr), image.Width, 0);
	}
}
