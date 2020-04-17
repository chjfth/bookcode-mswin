//------------------------------------------
// SimpleClock.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleClock: Form
{
     public static void Main()
     {
          Application.Run(new SimpleClock());
     }
     public SimpleClock()
     {
          Text = "Simple Clock";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          Timer timer    = new Timer();
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Interval = 1000;
          timer.Start();
     }
     private void TimerOnTick(object sender, EventArgs ea)
     {
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          StringFormat strfmt  = new StringFormat();
          strfmt.Alignment     = StringAlignment.Center;
          strfmt.LineAlignment = StringAlignment.Center;

          pea.Graphics.DrawString(DateTime.Now.ToString("F"),
                                  Font, new SolidBrush(ForeColor), 
                                  ClientRectangle, strfmt);
     }
}
