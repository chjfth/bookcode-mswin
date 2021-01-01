//-----------------------------------------
// HowdyWorld.cs (R) 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HowdyWorld: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new HowdyWorld());
     }
     public HowdyWorld()
     {
          Text = "Howdy, world!";
          MinimumSize = SystemInformation.MinimumWindowSize + new Size(0,1);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Font  font   = new Font("Times New Roman", 10, FontStyle.Italic);
          SizeF sizef  = grfx.MeasureString(Text, font);
          float fScale = Math.Min(cx / sizef.Width, cy / sizef.Height);

          font = new Font(font.Name, fScale * font.SizeInPoints, font.Style);
          sizef = grfx.MeasureString(Text, font);

          grfx.DrawString(Text, font, new SolidBrush(clr), 
                          (cx  - sizef.Width ) / 2, (cy - sizef.Height) / 2);
     }
}
