//-----------------------------------------------------
// StatusBarAndAutoScroll.cs © 2001 by Charles Petzold
//-----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class StatusBarAndAutoScroll: Form
{
     public static void Main()
     {
          Application.Run(new StatusBarAndAutoScroll());
     }
     public StatusBarAndAutoScroll()
     {
          Text = "Status Bar and Auto-Scroll";
          AutoScroll = true;

               // Create status bar.

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.Text = "My initial status bar text";

               // Create labels as children of the form.

          Label label = new Label();
          label.Parent = this;
          label.Text = "Upper left";
          label.Location = new Point(0, 0);

          label = new Label();
          label.Parent = this;
          label.Text = "Lower right";
          label.Location = new Point(250, 250);
          label.AutoSize = true;
     }
}
