//-----------------------------------------------
// RotateAndReflect.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class RotateAndReflect: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new RotateAndReflect());
     }
     public RotateAndReflect()
     {
          Text = "Rotated and Reflected Text";

          strText = "Reflect";
          font = new Font("Times New Roman", 36);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush   = new SolidBrush(clr);
          float        fAscent = GetAscent(grfx, font);
          StringFormat strfmt  = StringFormat.GenericTypographic;

          grfx.TranslateTransform(cx / 2, cy / 2);

          for (int i = 0; i < 4; i++)
          {
               GraphicsState grfxstate = grfx.Save();

               grfx.RotateTransform(-45);
               grfx.ScaleTransform((i > 1 ? -1 : 1), (i & 1) == 1 ? -1 : 1);
               grfx.DrawString(strText, font, brush, 0, -fAscent, strfmt); 
               grfx.Restore(grfxstate);
          }
     }
}