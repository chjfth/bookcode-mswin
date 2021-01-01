//------------------------------------------
// CaptureLoss.cs (R) 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CaptureLoss: Form
{
     public static void Main()
     {
          Application.Run(new CaptureLoss());
     }
     public CaptureLoss()
     {
          Text = "Capture Loss";

               // Hook up NativeWindow object.

          CaptureLossWindow win = new CaptureLossWindow();
          win.form = this;
          win.AssignHandle(Handle);
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          Invalidate();
     }
     public void OnLostCapture()
     {
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          if (Capture)
               grfx.FillRectangle(Brushes.Red, ClientRectangle);
          else
               grfx.FillRectangle(Brushes.Gray, ClientRectangle);
     }
}

class CaptureLossWindow: NativeWindow 
{
     public CaptureLoss form;

     protected override void WndProc(ref Message message) 
     {
          if (message.Msg == 533)                 // WM_CAPTURECHANGED
               form.OnLostCapture();

          base.WndProc(ref message);
     }
}
