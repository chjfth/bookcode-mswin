//------------- ----------------------
// Wink.cs © 2001 by Charles Petzold
//-----------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class Wink: Form
{
     protected Image[] aimage  = new Image[4];
     protected int     iImage = 0, iIncr = 1;

     public static void Main()
     {
          Application.Run(new Wink());
     }
     public Wink()
     {
          Text = "Wink";
          ResizeRedraw = true;
          BackColor = Color.White;

          for (int i = 0; i < 4; i++)
               aimage[i] = new Bitmap(GetType(), 
                                      "Wink.Eye" + (i + 1) + ".png");
          Timer timer = new Timer();
          timer.Interval = 100;
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Enabled = true;
     }
     protected virtual void TimerOnTick(object obj, EventArgs ea)
     {
          Graphics grfx = CreateGraphics();

          grfx.DrawImage(aimage[iImage], 
                        (ClientSize.Width  - aimage[iImage].Width)  / 2,
                        (ClientSize.Height - aimage[iImage].Height) / 2,
						aimage[iImage].Width, aimage[iImage].Height);
          grfx.Dispose();

          iImage += iIncr;

          if (iImage == 3)
               iIncr = -1;
          
          else if (iImage == 0)
               iIncr = 1;
     }
}
