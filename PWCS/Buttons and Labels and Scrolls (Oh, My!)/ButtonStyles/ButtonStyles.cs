//-------------------------------------------
// ButtonStyles.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ButtonStyles: Form
{
     public static void Main()
     {
          Application.Run(new ButtonStyles());
     }
     public ButtonStyles()
     {
          Text = "Button Styles";

          int y = 0;

          foreach (FlatStyle fs in Enum.GetValues(typeof(FlatStyle)))
          {
               Button btn    = new Button();
               btn.Parent    = this;
               btn.FlatStyle = fs;
               btn.Text      = fs.ToString();
               btn.Location  = new Point(50, y += 50);
          }
     }
}