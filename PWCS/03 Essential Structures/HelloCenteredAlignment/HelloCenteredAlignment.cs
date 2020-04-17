//-----------------------------------------------------
// HelloCenteredAlignment.cs © 2001 by Charles Petzold
//-----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HelloCenteredAlignment: Form
{
     public static void Main()
     {
          Application.Run(new HelloCenteredAlignment());
     }
     public HelloCenteredAlignment()
     {
          Text = "Hello Centered Using String Alignment";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics     grfx   = pea.Graphics;
          StringFormat strfmt = new StringFormat();

          strfmt.Alignment     = StringAlignment.Center;
          strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString("Hello, world!", Font, new SolidBrush(ForeColor), 
                          ClientSize.Width / 2, ClientSize.Height / 2, 
			              strfmt);
     }
}