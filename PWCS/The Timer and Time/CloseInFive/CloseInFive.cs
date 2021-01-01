//------------------------------------------
// CloseInFive.cs (R) 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CloseInFive: Form
{
     public static void Main()
     {
          Application.Run(new CloseInFive());
     }
     public CloseInFive()
     {
          Text = "Closing in Five Minutes";

          Timer timer    = new Timer();
          timer.Interval = 5 * 60 * 1000;
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Enabled  = true;
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          Timer timer = (Timer) obj;

          timer.Stop();
          timer.Tick -= new EventHandler(TimerOnTick);

          Close();          
     }
}
