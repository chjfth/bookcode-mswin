//-----------------------------------------
// EnterLeave.cs (R) 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class EnterLeave: Form
{
     bool bInside = false;

     public static void Main()
     {
          Application.Run(new EnterLeave());
     }
     public EnterLeave()
     {
          Text = "Enter/Leave";
     }
     protected override void OnMouseEnter(EventArgs ea)
     {
          bInside = true;
          Invalidate();
     }
     protected override void OnMouseLeave(EventArgs ea)
     {
          bInside = false;
          Invalidate();
     }
     protected override void OnMouseHover(EventArgs ea)
     {
          Graphics grfx = CreateGraphics();
    
          grfx.Clear(Color.Red);
          System.Threading.Thread.Sleep(100);
          grfx.Clear(Color.Green);
          grfx.Dispose();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.Clear(bInside ? Color.Green : BackColor);
     }
}
