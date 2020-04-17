//-------------------------------------------------
// ScribbleWithBitmap.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ScribbleWithBitmap: Form
{
     bool     bTracking;
     Point    ptLast;
     Bitmap   bitmap;
     Graphics grfxBm;

     public static void Main()
     {
          Application.Run(new ScribbleWithBitmap());
     }
     public ScribbleWithBitmap()
     {
          Text = "Scribble with Bitmap";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create bitmap

          Size size = SystemInformation.PrimaryMonitorMaximizedWindowSize;
          bitmap = new Bitmap(size.Width, size.Height);

               // Create Graphics object from bitmap

          grfxBm = Graphics.FromImage(bitmap);
          grfxBm.Clear(BackColor);
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (mea.Button != MouseButtons.Left)
               return;

          ptLast = new Point(mea.X, mea.Y);
          bTracking = true;
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          if (!bTracking)
               return;

          Point ptNew = new Point(mea.X, mea.Y);
          
          Pen pen = new Pen(ForeColor);
          Graphics grfx = CreateGraphics();
          grfx.DrawLine(pen, ptLast, ptNew);
          grfx.Dispose();

               // Draw on bitmap

          grfxBm.DrawLine(pen, ptLast, ptNew);

          ptLast = ptNew;
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          bTracking = false;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

               // Display bitmap

          grfx.DrawImage(bitmap, 0, 0, bitmap.Width, bitmap.Height);
     }
}

