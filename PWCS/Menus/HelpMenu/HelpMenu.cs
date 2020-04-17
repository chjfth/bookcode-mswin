//---------------------------------------
// HelpMenu.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HelpMenu: Form
{
     Bitmap bmHelp;

     public static void Main()
     {
          Application.Run(new HelpMenu());
     }
     public HelpMenu()
     {
          Text = "Help Menu";

          bmHelp = new Bitmap(GetType(), "HelpMenu.Bighelp.bmp");

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Help");

          MenuItem mi     = new MenuItem("&Help");
          mi.OwnerDraw    = true;
          mi.Click       += new EventHandler(MenuHelpOnClick);
          mi.DrawItem    += new DrawItemEventHandler(MenuHelpOnDrawItem);
          mi.MeasureItem += 
                    new MeasureItemEventHandler(MenuHelpOnMeasureItem);

          Menu.MenuItems[0].MenuItems.Add(mi);
     }
     void MenuHelpOnMeasureItem(object obj, MeasureItemEventArgs miea)
     {
          miea.ItemWidth  = bmHelp.Width;
          miea.ItemHeight = bmHelp.Height;
     }
     void MenuHelpOnDrawItem(object obj, DrawItemEventArgs diea)
     {
          Rectangle rect = diea.Bounds;
          rect.X += diea.Bounds.Width - bmHelp.Width;
          rect.Width = bmHelp.Width;

          diea.DrawBackground();
          diea.Graphics.DrawImage(bmHelp, rect);
     }
     void MenuHelpOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Help not yet implemented.", Text);
     }
}




