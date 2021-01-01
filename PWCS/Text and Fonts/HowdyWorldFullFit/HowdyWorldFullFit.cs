//------------------------------------------------
// HowdyWorldFullFit.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HowdyWorldFullFit: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new HowdyWorldFullFit());
     }
     public HowdyWorldFullFit()
     {
          Text = "Howdy, world!";
          MinimumSize = SystemInformation.MinimumWindowSize + new Size(0,1);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Font  font  = new Font("Times New Roman", 10, FontStyle.Italic);
          SizeF sizef = grfx.MeasureString(Text, font);
          float fScaleHorz = cx / sizef.Width;
          float fScaleVert = cy / sizef.Height;

          grfx.ScaleTransform(fScaleHorz, fScaleVert);

          grfx.DrawString(Text, font, new SolidBrush(clr), 0, 0);
     }
}
