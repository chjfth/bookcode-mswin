//------------------------------------------
// BezierClock.cs (R) 2001 by Charles Petzold
//------------------------------------------
using Petzold.ProgrammingWindowsWithCSharp;
using System;
using System.Drawing;
using System.Windows.Forms;

class BezierClock: Form
{
     BezierClockControl clkctl;

     public static void Main()
     {
          Application.Run(new BezierClock());
     }
     public BezierClock()
     {
          Text = "Bezier Clock";

          clkctl = new BezierClockControl();
          clkctl.Parent    = this;
          clkctl.Time      = DateTime.Now;
          clkctl.Dock      = DockStyle.Fill;
          clkctl.BackColor = Color.Black;
          clkctl.ForeColor = Color.White;

          Timer timer = new Timer();
          timer.Interval = 100;
          timer.Tick += new EventHandler(OnTimerTick);
          timer.Start();
     }
     void OnTimerTick(object obj, EventArgs ea)
     {
          clkctl.Time = DateTime.Now;
     }
}
