//----------------------------------------------------
// HelloCenteredMeasured.cs (R) 2001 by Charles Petzold
//----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HelloCenteredMeasured: Form
{
     public static void Main() 
     {
          Application.Run(new HelloCenteredMeasured()); 
     }
     public HelloCenteredMeasured()
     {
          Text = "Hello Centered Using MeasureString";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx      = pea.Graphics;
          string   str       = "Hello, world!";
          SizeF    sizefText = grfx.MeasureString(str, Font);

          grfx.DrawString(str, Font, new SolidBrush(ForeColor), 
                          (ClientSize.Width  - sizefText.Width)  / 2, 
                          (ClientSize.Height - sizefText.Height) / 2);
     }
}