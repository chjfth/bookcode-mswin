//------------------------------------------------
// DateAndTimeStatus.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DateAndTimeStatus: Form
{
     StatusBarPanel sbpMenu, sbpDate, sbpTime;

     public static void Main()
     {
          Application.Run(new DateAndTimeStatus());
     }
     public DateAndTimeStatus()
     {
          Text = "Date and Time Status";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create status bar.

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.ShowPanels = true;

               // Create status bar panels.

          sbpMenu = new StatusBarPanel();
          sbpMenu.Text = "Reserved for menu help";
          sbpMenu.BorderStyle = StatusBarPanelBorderStyle.None;
          sbpMenu.AutoSize = StatusBarPanelAutoSize.Spring;

          sbpDate = new StatusBarPanel();
          sbpDate.AutoSize = StatusBarPanelAutoSize.Contents;
          sbpDate.ToolTipText = "The current date";
          
          sbpTime = new StatusBarPanel();
          sbpTime.AutoSize = StatusBarPanelAutoSize.Contents;
          sbpTime.ToolTipText = "The current time";

               // Attach status bar panels to status bar.

          sb.Panels.AddRange(new StatusBarPanel[] 
				                    { sbpMenu, sbpDate, sbpTime });

               // Set the timer for 1 second.

          Timer timer = new Timer();
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Interval = 1000;
          timer.Start();
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          DateTime dt = DateTime.Now;

          sbpDate.Text = dt.ToShortDateString();
          sbpTime.Text = dt.ToShortTimeString();
     }
}
