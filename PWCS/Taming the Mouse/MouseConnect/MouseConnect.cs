// ------------------------------------------
// MouseConnect.cs © 2001 by Charles Petzold
// ------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MouseConnect: Form
{
     const int iMaxPoints = 1000;
     int       iNumPoints = 0;
     Point[]   apoint     = new Point[iMaxPoints];

     public static void Main()
     {
          Application.Run(new MouseConnect());
     }
     public MouseConnect()
     {
          Text = "Mouse Connect: Press, drag quickly, release";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ClientSize += ClientSize;     // Double the client area.
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (mea.Button == MouseButtons.Left)
          {
               iNumPoints = 0;
               Invalidate();
          }
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          if (mea.Button == MouseButtons.Left)
          {
               apoint[iNumPoints++] = new Point(mea.X, mea.Y);

               Graphics grfx = CreateGraphics();
               grfx.DrawLine(new Pen(ForeColor), mea.X, mea.Y, 
                                                 mea.X, mea.Y + 1);
			  grfx.Dispose();
          }
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          if (mea.Button == MouseButtons.Left)
               Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          for (int i = 0   ; i < iNumPoints - 1; i++)
               for (int j = i + 1; j < iNumPoints; j++)
                    grfx.DrawLine(pen, apoint[i], apoint[j]);
    }
}
