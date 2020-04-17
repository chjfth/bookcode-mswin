//----------------------------------------
// RoundRect.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RoundRect: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new RoundRect());
     }
     public RoundRect()
     {
          Text = "Rounded Rectangle";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          RoundedRectangle(grfx, Pens.Red, 
                           new Rectangle(0, 0, cx - 1, cy - 1),
                           new Size(cx / 5, cy / 5));
     }
     void RoundedRectangle(Graphics grfx, Pen pen, Rectangle rect, Size size)
     {
          grfx.DrawLine(pen, rect.Left  + size.Width / 2, rect.Top,
                             rect.Right - size.Width / 2, rect.Top);

          grfx.DrawArc(pen, rect.Right - size.Width, rect.Top, 
                            size.Width, size.Height, 270, 90);

          grfx.DrawLine(pen, rect.Right, rect.Top + size.Height / 2,
                             rect.Right, rect.Bottom - size.Height / 2);

          grfx.DrawArc(pen, rect.Right - size.Width, 
                            rect.Bottom - size.Height,
                            size.Width, size.Height, 0, 90);

          grfx.DrawLine(pen, rect.Right - size.Width / 2, rect.Bottom,
                             rect.Left + size.Width / 2, rect.Bottom);

          grfx.DrawArc(pen, rect.Left, rect.Bottom - size.Height,
                            size.Width, size.Height, 90, 90);

          grfx.DrawLine(pen, rect.Left, rect.Bottom - size.Height / 2,
                             rect.Left, rect.Top + size.Height / 2);

          grfx.DrawArc(pen, rect.Left, rect.Top,
                            size.Width, size.Height, 180, 90);
     }
}
