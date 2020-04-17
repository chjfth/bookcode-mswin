//--------------------------------------------------
// KeyholeClipCentered.cs © 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class KeyholeClipCentered: KeyholeClip
{
     public new static void Main()
     {
          Application.Run(new KeyholeClipCentered());
     }
     public KeyholeClipCentered()
     {
          Text += " Centered";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.SetClip(path);

          RectangleF rectf   = path.GetBounds();
          int        xOffset = (int)((cx - rectf.Width)  / 2 - rectf.X);
          int        yOffset = (int)((cy - rectf.Height) / 2 - rectf.Y);

          grfx.TranslateClip(xOffset, yOffset);
          grfx.DrawImage(image, xOffset, yOffset, image.Width, image.Height);
     }
}
