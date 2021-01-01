//----------------------------------------------
// SimpleStatusBar.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleStatusBar: Form
{
     public static void Main()
     {
          Application.Run(new SimpleStatusBar());
     }
     public SimpleStatusBar()
     {
          Text = "Simple Status Bar";
		  ResizeRedraw = true;

               // Create status bar.

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.Text = "My initial status bar text";
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          grfx.DrawLine(pen, 0, 0, ClientSize.Width, ClientSize.Height);
          grfx.DrawLine(pen, ClientSize.Width, 0, 0, ClientSize.Height);
     }
}
