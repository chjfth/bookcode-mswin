//--------------------------------------------
// PenDashStyles.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class PenDashStyles: PrintableForm
{
     MenuItem miChecked;

     public new static void Main()
     {
          Application.Run(new PenDashStyles());
     }
     public PenDashStyles()
     {
          Text = "Pen Dash Styles";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Width");

          int[] aiWidth = { 1, 2, 5, 10, 15, 20, 25 };

          foreach (int iWidth in aiWidth)
               Menu.MenuItems[0].MenuItems.Add(iWidth.ToString(), 
                                        new EventHandler(MenuWidthOnClick));

          miChecked = Menu.MenuItems[0].MenuItems[0];
          miChecked.Checked = true;
     }
     void MenuWidthOnClick(object obj, EventArgs ea)
     {
          miChecked.Checked = false;
          miChecked = (MenuItem) obj;
          miChecked.Checked = true;
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Pen pen = new Pen(clr);
          pen.Width = Convert.ToInt32(miChecked.Text);

          for (int i = 0; i < 5; i++)
          {
               pen.DashStyle = (DashStyle) i;

               int y = (i + 1) * cy / 6;

               grfx.DrawLine(pen, cx / 8, y, 7 * cx / 8, y); 
          }
     }
}
