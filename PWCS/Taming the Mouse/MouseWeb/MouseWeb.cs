//---------------------------------------
// MouseWeb.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MouseWeb: Form
{
     Point ptMouse = Point.Empty;

     public static void Main()
     {
          Application.Run(new MouseWeb());
     }
     public MouseWeb()
     {
          Text = "Mouse Web";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          Graphics grfx = CreateGraphics();

          DrawWeb(grfx, BackColor, ptMouse);
          ptMouse = new Point(mea.X, mea.Y);
          DrawWeb(grfx, ForeColor, ptMouse);

          grfx.Dispose();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          DrawWeb(pea.Graphics, ForeColor, ptMouse);
     }
     void DrawWeb(Graphics grfx, Color clr, Point pt)
     {
          int cx  = ClientSize.Width;
          int cy  = ClientSize.Height;
          Pen pen = new Pen(clr);

          grfx.DrawLine(pen, pt, new Point(         0,          0));
          grfx.DrawLine(pen, pt, new Point(    cx / 4,          0));
          grfx.DrawLine(pen, pt, new Point(    cx / 2,          0));
          grfx.DrawLine(pen, pt, new Point(3 * cx / 4,          0));
          grfx.DrawLine(pen, pt, new Point(    cx    ,          0));
          grfx.DrawLine(pen, pt, new Point(    cx    ,     cy / 4));
          grfx.DrawLine(pen, pt, new Point(    cx    ,     cy / 2));
          grfx.DrawLine(pen, pt, new Point(    cx    , 3 * cy / 4));
          grfx.DrawLine(pen, pt, new Point(    cx    ,     cy    ));
          grfx.DrawLine(pen, pt, new Point(3 * cx / 4,     cy    ));
          grfx.DrawLine(pen, pt, new Point(    cx / 2,     cy    ));
          grfx.DrawLine(pen, pt, new Point(    cx / 4,     cy    ));
          grfx.DrawLine(pen, pt, new Point(         0,     cy    ));
          grfx.DrawLine(pen, pt, new Point(         0,     cy / 4));
          grfx.DrawLine(pen, pt, new Point(         0,     cy / 2));
          grfx.DrawLine(pen, pt, new Point(         0, 3 * cy / 4));
     }
}
