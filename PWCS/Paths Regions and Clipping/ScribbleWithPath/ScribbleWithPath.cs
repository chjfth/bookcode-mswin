//-----------------------------------------------
// ScribbleWithPath.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class ScribbleWithPath: Form
{
     GraphicsPath path;
     bool         bTracking;
     Point        ptLast;

     public static void Main()
     {
          Application.Run(new ScribbleWithPath());
     }
     public ScribbleWithPath()
     {
          Text = "Scribble with Path";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create the path.

          path = new GraphicsPath();
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (mea.Button != MouseButtons.Left)
               return;

          ptLast = new Point(mea.X, mea.Y);
          bTracking = true;

               // Start a figure.

          path.StartFigure();
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          if (!bTracking)
               return;

          Point ptNew = new Point(mea.X, mea.Y);
          
          Graphics grfx = CreateGraphics();
          grfx.DrawLine(new Pen(ForeColor), ptLast, ptNew);
          grfx.Dispose();

               // Add a line.

          path.AddLine(ptLast, ptNew);

          ptLast = ptNew;
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          bTracking = false;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
               // Draw the path.

          pea.Graphics.DrawPath(new Pen(ForeColor), path);
     }
}
