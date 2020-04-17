//-------------------------------------------
// AnalogClock.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;
using Petzold.ProgrammingWindowsWithCSharp;

class AnalogClock: Form
{
     ClockControl clkctl;

     public static void Main()
     {
          Application.Run(new AnalogClock());
     }
     public AnalogClock()
     {
          Text = "Analog Clock";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          clkctl = new ClockControl();
          clkctl.Parent    = this;
          clkctl.Time      = DateTime.Now;
          clkctl.Dock      = DockStyle.Fill;
          clkctl.BackColor = Color.Black;
          clkctl.ForeColor = Color.White;

          Timer timer    = new Timer();
          timer.Interval = 100;
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Start();
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          clkctl.Time = DateTime.Now;
     }
}
