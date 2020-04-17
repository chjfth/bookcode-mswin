//-----------------------------------------------------------
// RectangleLinearGradientBrush.cs © 2001 by Charles Petzold
//-----------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class RectangleLinearGradientBrush: PrintableForm
{
     MenuItem miChecked;

     public new static void Main()
     {
          Application.Run(new RectangleLinearGradientBrush());
     } 
     public RectangleLinearGradientBrush()
     {
          Text = "Rectangle Linear-Gradient Brush";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Gradient-Mode");

          foreach (LinearGradientMode gm in 
                              Enum.GetValues(typeof(LinearGradientMode)))
          {
               MenuItem mi = new MenuItem();
               mi.Text     = gm.ToString(); 
               mi.Click   += new EventHandler(MenuGradientModeOnClick);
               Menu.MenuItems[0].MenuItems.Add(mi);
          }
          miChecked = Menu.MenuItems[0].MenuItems[0];
          miChecked.Checked = true;
     }
     void MenuGradientModeOnClick(object obj, EventArgs ea)
     {
          miChecked.Checked = false;
          miChecked = (MenuItem) obj;
          miChecked.Checked = true;
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Rectangle rectBrush = 
                         new Rectangle(cx / 4, cy / 4, cx / 2, cy / 2);

          LinearGradientBrush lgbrush = 
               new LinearGradientBrush(
                         rectBrush, Color.White, Color.Black,
                         (LinearGradientMode) miChecked.Index);

         grfx.FillRectangle(lgbrush, 0, 0, cx, cy);
         grfx.DrawRectangle(Pens.Black, rectBrush);
     }
}
