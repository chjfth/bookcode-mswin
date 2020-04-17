//-------------------------------------------
// TiltedShadow.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TiltedShadow: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new TiltedShadow());
     }
     public TiltedShadow()
     {
          Text = "Tilted Shadow";

          strText = "Shadow";
          font = new Font("Times New Roman", 54);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          float fAscent = GetAscent(grfx, font);

               // Set baseline 3/4 down client area.

          grfx.TranslateTransform(0, 3 * cy / 4);

               // Save the graphics state.

          GraphicsState grfxstate = grfx.Save();

               // Set scaling and shear, and draw shadow.

          grfx.MultiplyTransform(new Matrix(1, 0, -3, 3, 0, 0)); 
          grfx.DrawString(strText, font, Brushes.DarkGray, 0, -fAscent);

               // Draw text without scaling or shear.

          grfx.Restore(grfxstate);
          grfx.DrawString(strText, font, Brushes.Black, 0, -fAscent);
     }
}

