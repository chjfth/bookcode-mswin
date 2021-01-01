//-------------------------------------------------
// PictureBoxPlusDemo.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using Petzold.ProgrammingWindowsWithCSharp;
using System;
using System.Drawing;
using System.Windows.Forms;

class PictureBoxPlusDemo: Form
{
	public static void Main()
	{
		Application.Run(new PictureBoxPlusDemo());
	}
	public PictureBoxPlusDemo()
	{
		Text = "PictureBoxPlus Demo";

		PictureBoxPlus picbox = new PictureBoxPlus();
		picbox.Parent = this;
		picbox.Dock = DockStyle.Fill;
		picbox.Image = Image.FromFile("..\\..\\..\\Apollo11FullColor.jpg");
		picbox.SizeMode = PictureBoxSizeMode.StretchImage;
		picbox.NoDistort = true;
	}
}
