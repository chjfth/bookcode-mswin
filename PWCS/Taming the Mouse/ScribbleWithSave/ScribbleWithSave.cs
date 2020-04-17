//-----------------------------------------------
// ScribbleWithSave.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Collections;          // For ArrayList
using System.Drawing;
using System.Windows.Forms;

class ScribbleWithSave: Form
{
     ArrayList arrlstApts = new ArrayList();
     ArrayList arrlstPts;
     bool      bTracking;

     public static void Main()
     {
          Application.Run(new ScribbleWithSave());
     }
     public ScribbleWithSave()
     {
          Text = "Scribble with Save";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (mea.Button != MouseButtons.Left)
               return;

          arrlstPts = new ArrayList();
          arrlstPts.Add(new Point(mea.X, mea.Y));

          bTracking = true;
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          if (!bTracking)
               return;

          arrlstPts.Add(new Point(mea.X, mea.Y));
 
          Graphics grfx = CreateGraphics();
          grfx.DrawLine(new Pen(ForeColor),
                        (Point) arrlstPts[arrlstPts.Count - 2],
                        (Point) arrlstPts[arrlstPts.Count - 1]);
          grfx.Dispose();
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          if (!bTracking)
               return;

          Point[] apt = (Point[]) arrlstPts.ToArray(typeof(Point));
          arrlstApts.Add(apt);
          bTracking = false;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          for (int i = 0; i < arrlstApts.Count; i++)
               grfx.DrawLines(pen, (Point[]) arrlstApts[i]);
     }
}
