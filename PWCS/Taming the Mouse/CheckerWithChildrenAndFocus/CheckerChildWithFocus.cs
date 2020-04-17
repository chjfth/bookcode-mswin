//----------------------------------------------------
// CheckerChildWithFocus.cs © 2001 by Charles Petzold
//----------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class CheckerChildWithFocus: CheckerChild
{
     protected override void OnGotFocus(EventArgs ea)
     {
          Invalidate();
     }
     protected override void OnLostFocus(EventArgs ea)
     {
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          base.OnPaint(pea);

          if (Focused)
          {
               Graphics grfx = pea.Graphics;
               grfx.DrawRectangle(new Pen(ForeColor, 5), ClientRectangle);
          }
     }
}
