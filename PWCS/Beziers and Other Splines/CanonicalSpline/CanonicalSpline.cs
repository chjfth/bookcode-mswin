//----------------------------------------------
// CanonicalSpline.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CanonicalSpline: Form
{
     protected Point[] apt      = new Point[4];
     protected float   fTension = 0.5f;

     public static void Main()
     {
          Application.Run(new CanonicalSpline());
     }
     public CanonicalSpline()
     {
          Text = "Canonical Spline";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;

          ScrollBar scroll = new VScrollBar();
          scroll.Parent = this;
          scroll.Dock = DockStyle.Right;
          scroll.Minimum = -100;
          scroll.Maximum =  109;
          scroll.SmallChange =  1;
          scroll.LargeChange = 10;
          scroll.Value = (int) (10 * fTension);
          scroll.ValueChanged += new EventHandler(ScrollOnValueChanged);

		  OnResize(EventArgs.Empty);
     }
     void ScrollOnValueChanged(object obj, EventArgs ea)
     {
          ScrollBar scroll = (ScrollBar) obj;

          fTension = scroll.Value / 10f;

          Invalidate(false);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          int cx = ClientSize.Width;
          int cy = ClientSize.Height;

          apt[0] = new Point(    cx / 4,     cy / 2);
          apt[1] = new Point(    cx / 2,     cy / 4);
          apt[2] = new Point(    cx / 2, 3 * cy / 4);
          apt[3] = new Point(3 * cx / 4,     cy / 2);
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          Point pt;

          if (mea.Button == MouseButtons.Left)
          {
               if (ModifierKeys == Keys.Shift)
                    pt = apt[0];
               else if (ModifierKeys == Keys.None)
                    pt = apt[1];
               else
                    return;
          }
          else if (mea.Button == MouseButtons.Right)
          {
               if (ModifierKeys == Keys.None)
                    pt = apt[2];
               else if (ModifierKeys == Keys.Shift)
                    pt = apt[3];
               else
                    return;
          }
          else
               return;

          Cursor.Position = PointToScreen(pt);
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          Point pt = new Point(mea.X, mea.Y);

          if (mea.Button == MouseButtons.Left)
          {
               if (ModifierKeys == Keys.Shift)
                    apt[0] = pt;
               else if (ModifierKeys == Keys.None)
                    apt[1] = pt;
               else
                    return;
          }
          else if (mea.Button == MouseButtons.Right)
          {
               if (ModifierKeys == Keys.None)
                    apt[2]= pt;
               else if (ModifierKeys == Keys.Shift)
                    apt[3] = pt;
               else
                    return;
          }
          else
               return;

          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx  = pea.Graphics;
          Brush    brush = new SolidBrush(ForeColor);

          grfx.DrawCurve(new Pen(ForeColor), apt, fTension);

          grfx.DrawString("Tension = " + fTension, Font, brush, 0, 0);

          for (int i = 0; i < 4; i++)
               grfx.FillEllipse(brush, apt[i].X - 3, apt[i].Y - 3, 7, 7);
     }
}
