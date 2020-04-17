//----------------------------------------------
// RandomRectangle.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RandomRectangle: Form
{
     public static void Main()
     {
          Application.Run(new RandomRectangle());
     }
     public RandomRectangle()
     {
          Text = "Random Rectangle";

          Timer timer    = new Timer();
          timer.Interval = 1;
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Start();
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          Random rand = new Random();

          int x1 = rand.Next(ClientSize.Width);
          int x2 = rand.Next(ClientSize.Width);
          int y1 = rand.Next(ClientSize.Height);
          int y2 = rand.Next(ClientSize.Height);

          Color color = Color.FromArgb(rand.Next(256), 
                                       rand.Next(256), 
                                       rand.Next(256));

          Graphics grfx = CreateGraphics();
          grfx.FillRectangle(new SolidBrush(color), 
                             Math.Min(x1,  x2), Math.Min(y1,  y2),
                             Math.Abs(x2 - x1), Math.Abs(y2 - y1));
          grfx.Dispose();
     }
}
