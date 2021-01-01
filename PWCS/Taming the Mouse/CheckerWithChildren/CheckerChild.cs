//-------------------------------------------
// CheckerChild.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CheckerChild: UserControl
{
     bool bChecked = false;

     public CheckerChild()
     {
          ResizeRedraw = true;
     }
     protected override void OnClick(EventArgs ea)
     {
          base.OnClick(ea);

          bChecked = !bChecked;
          Invalidate();
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          switch(kea.KeyCode)
          {
          case Keys.Enter:
          case Keys.Space:
               OnClick(new EventArgs());
               break;
          }
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          grfx.DrawRectangle(pen, ClientRectangle);

          if (bChecked)
          {
               grfx.DrawLine(pen, 0, 0, ClientSize.Width, ClientSize.Height);
               grfx.DrawLine(pen, 0, ClientSize.Height, ClientSize.Width, 0);
          }
     }
}
