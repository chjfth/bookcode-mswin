//-------------------------------------
// Bounce.cs (R) 2001 by Charles Petzold
//-------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class Bounce: Form
{
     const  int iTimerInterval = 25;    // In milliseconds
     const  int iBallSize = 16;         // As fraction of client area
     const  int iMoveSize = 4;          // As fraction of ball size

     Bitmap bitmap;
     int    xCenter, yCenter;
     int    cxRadius, cyRadius, cxMove, cyMove, cxTotal, cyTotal;

     public static void Main()
     {
          Application.Run(new Bounce());
     }
     public Bounce()
     {
          Text = "Bounce";
          ResizeRedraw = true;
          BackColor = Color.White;

          Timer timer = new Timer();
          timer.Interval = iTimerInterval;
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Start();

		  OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          Graphics grfx = CreateGraphics();
          grfx.Clear(BackColor);

          float fRadius = Math.Min(ClientSize.Width  / grfx.DpiX,
                                   ClientSize.Height / grfx.DpiY) 
                                        / iBallSize;

          cxRadius = (int) (fRadius * grfx.DpiX);
          cyRadius = (int) (fRadius * grfx.DpiY);

          grfx.Dispose();
               
          cxMove = Math.Max(1, cxRadius / iMoveSize);
          cyMove = Math.Max(1, cyRadius / iMoveSize);

          cxTotal = 2 * (cxRadius + cxMove);
          cyTotal = 2 * (cyRadius + cyMove);

          bitmap = new Bitmap(cxTotal, cyTotal);

          grfx = Graphics.FromImage(bitmap);
          grfx.Clear(BackColor);

          DrawBall(grfx, new Rectangle(cxMove, cyMove, 
                                       2 * cxRadius, 2 * cyRadius));
          grfx.Dispose();

          xCenter = ClientSize.Width / 2;
          yCenter = ClientSize.Height / 2;
     }
     protected virtual void DrawBall(Graphics grfx, Rectangle rect)
     {
          grfx.FillEllipse(Brushes.Red, rect);
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          Graphics grfx = CreateGraphics();

          grfx.DrawImage(bitmap, xCenter - cxTotal / 2,
                         yCenter - cyTotal / 2,
						 cxTotal, cyTotal);
          grfx.Dispose();

          xCenter += cxMove;
          yCenter += cyMove;

          if ((xCenter + cxRadius >= ClientSize.Width) || 
              (xCenter - cxRadius <= 0))
                    cxMove = -cxMove;

          if ((yCenter + cyRadius >= ClientSize.Height) || 
              (yCenter - cyRadius <= 0))
                    cyMove = -cyMove;
     }
}
