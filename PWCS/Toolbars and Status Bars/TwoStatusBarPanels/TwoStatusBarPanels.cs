//-------------------------------------------------
// TwoStatusBarPanels.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwoStatusBarPanels: Form
{
     public static void Main()
     {
          Application.Run(new TwoStatusBarPanels());
     }
     public TwoStatusBarPanels()
     {
          Text = "Two Status Bar Panels";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.ShowPanels = true;

          StatusBarPanel sbpanel1 = new StatusBarPanel();
          sbpanel1.Text = "Panel 1";

          StatusBarPanel sbpanel2 = new StatusBarPanel();
          sbpanel2.Text = "Panel 2";

          sb.Panels.Add(sbpanel1);
          sb.Panels.Add(sbpanel2);
     }
}
