//--------------------------------------------------
// ModelessColorScroll.cs (R) 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ModelessColorScroll: Form
{
     public static void Main()
     {
          Application.Run(new ModelessColorScroll());
     }
     public ModelessColorScroll()
     {
          Text = "Modeless Color Scroll";

          ColorScrollDialogBox dlg = new ColorScrollDialogBox();
          
          dlg.Owner = this;
          dlg.Color = BackColor;
          dlg.Changed += new EventHandler(ColorScrollOnChanged);
          dlg.Show();
     }
     void ColorScrollOnChanged(object obj, EventArgs ea)
     {
          ColorScrollDialogBox dlg = (ColorScrollDialogBox) obj;

          BackColor = dlg.Color;
     }
}