//------------------------------------------------
// SevenSegmentClock.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Globalization;
using System.Windows.Forms;
using Petzold.ProgrammingWindowsWithCSharp;

class SevenSegmentClock: Form
{
     DateTime dt;

     public static void Main()
     {
          Application.Run(new SevenSegmentClock());
     }
     public SevenSegmentClock()
     {
          Text = "Seven-Segment Clock";
          BackColor = Color.White;
          ResizeRedraw = true;
          MinimumSize = SystemInformation.MinimumWindowSize + new Size(0, 1);

          dt = DateTime.Now;

          Timer timer    = new Timer();
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Interval = 100;
          timer.Enabled  = true;
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          DateTime dtNow = DateTime.Now;
          dtNow = new DateTime(dtNow.Year, dtNow.Month, dtNow.Day,
                               dtNow.Hour, dtNow.Minute, dtNow.Second);
          if (dtNow != dt)
          {
               dt = dtNow;
               Invalidate();
          }
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          SevenSegmentDisplay ssd = new SevenSegmentDisplay(pea.Graphics);

          string strTime = dt.ToString("T", 
                                       DateTimeFormatInfo.InvariantInfo);
          SizeF  sizef   = ssd.MeasureString(strTime, Font);
          float  fScale  = Math.Min(ClientSize.Width  / sizef.Width,
                                    ClientSize.Height / sizef.Height);
          Font   font    = new Font(Font.FontFamily, 
                                    fScale * Font.SizeInPoints);

          sizef = ssd.MeasureString(strTime, font);

          ssd.DrawString(strTime, font, Brushes.Red, 
                         (ClientSize.Width  - sizef.Width) / 2, 
                         (ClientSize.Height - sizef.Height) / 2);
     }
}
