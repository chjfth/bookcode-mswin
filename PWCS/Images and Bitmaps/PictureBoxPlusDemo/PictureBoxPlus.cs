//---------------------------------------------
// PictureBoxPlus.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

namespace Petzold.ProgrammingWindowsWithCSharp
{
	class PictureBoxPlus: PictureBox
	{
		bool bNoDistort = false;

		public bool NoDistort
		{
			get { return bNoDistort; }
			set 
			{
				bNoDistort = value;
				Invalidate();
			}
		}    
		protected override void OnPaint(PaintEventArgs pea)
		{
			if ((Image !=  null) && NoDistort &&
				(SizeMode == PictureBoxSizeMode.StretchImage))
				ScaleImageIsotropically(pea.Graphics, Image, ClientRectangle);
			else
				base.OnPaint(pea);
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
}
