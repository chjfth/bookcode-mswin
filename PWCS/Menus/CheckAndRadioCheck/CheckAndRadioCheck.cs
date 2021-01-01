//-------------------------------------------------
// CheckAndRadioCheck.cs (R) 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CheckAndRadioCheck: Form
{
     MenuItem miColor, miFill;

     public static void Main()
     {
          Application.Run(new CheckAndRadioCheck());
     }
     public CheckAndRadioCheck()
     {
          Text = "Check and Radio Check";
          ResizeRedraw = true;

          string[]     astrColor = {"Black", "Blue",    "Green",  "Cyan",
                                    "Red",   "Magenta", "Yellow", "White"};
          MenuItem[]   ami       = new MenuItem[astrColor.Length + 2];
          EventHandler ehColor   = new EventHandler(MenuFormatColorOnClick);

          for (int i = 0; i < astrColor.Length; i++)
          {
               ami[i] = new MenuItem(astrColor[i], ehColor);
               ami[i].RadioCheck = true;
          }
          miColor = ami[0];
          miColor.Checked = true;

          ami[astrColor.Length] = new MenuItem("-");
          
          miFill = new MenuItem("&Fill",
                                new EventHandler(MenuFormatFillOnClick));

          ami[astrColor.Length + 1] = miFill;

          MenuItem mi = new MenuItem("&Format", ami);
          
          Menu = new MainMenu(new MenuItem[] {mi});
     }
     void MenuFormatColorOnClick(object obj, EventArgs ea)
     {
          miColor.Checked = false;
          miColor = (MenuItem)obj;
          miColor.Checked = true;

          Invalidate();
     }
     void MenuFormatFillOnClick(object obj, EventArgs ea)
     {
          MenuItem mi = (MenuItem)obj;

          mi.Checked ^= true;

          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          if (miFill.Checked)
          {
               Brush brush = new SolidBrush(Color.FromName(miColor.Text));
               grfx.FillEllipse(brush, 0, 0, ClientSize.Width - 1,
                                             ClientSize.Height - 1);
          }
          else
          {
               Pen pen = new Pen(Color.FromName(miColor.Text));
               grfx.DrawEllipse(pen, 0, 0, ClientSize.Width - 1,
                                           ClientSize.Height - 1);
          }
     }
}