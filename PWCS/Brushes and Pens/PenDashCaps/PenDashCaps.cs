//------------------------------------------
// PenDashCaps.cs (R) 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class PenDashCaps: PrintableForm
{
     MenuItem miChecked;

     public new static void Main()
     {
          Application.Run(new PenDashCaps());
     }
     public PenDashCaps()
     {
          Text = "Pen Dash Caps: Flat, Round, Triangle";

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
          Pen pen = new Pen(clr, Convert.ToInt32(miChecked.Text));
          pen.DashStyle = DashStyle.DashDotDot;

          foreach (DashCap dc in Enum.GetValues(typeof(DashCap)))
          {
               pen.DashCap = dc;

               grfx.DrawLine(pen, cx / 8, cy / 4, 7 * cx / 8, cy / 4); 
               grfx.TranslateTransform(0, cy / 4);
          }
     }
}
