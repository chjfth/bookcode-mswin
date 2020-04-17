//-------------------------------------------------------
// SimpleStatusBarWithPanel.cs © 2001 by Charles Petzold
//-------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleStatusBarWithPanel: Form
{
     public static void Main()
     {
          Application.Run(new SimpleStatusBarWithPanel());
     }
     public SimpleStatusBarWithPanel()
     {
          Text = "Simple Status Bar with Panel";

               // Create panel.

          Panel panel = new Panel();
          panel.Parent = this;
          panel.BackColor = SystemColors.Window;
          panel.ForeColor = SystemColors.WindowText;
          panel.AutoScroll = true;
          panel.Dock = DockStyle.Fill;
          panel.BorderStyle = BorderStyle.Fixed3D;

               // Create status bar as child of form.

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.Text = "My initial status bar text";

               // Create labels as children of panel.

          Label label = new Label();
          label.Parent = panel;
          label.Text = "Upper left";
          label.Location = new Point(0, 0);

          label = new Label();
          label.Parent = panel;
          label.Text = "Lower right";
          label.Location = new Point(250, 250);
          label.AutoSize = true;
     }
}
