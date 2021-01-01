//------------------------------------------------
// BetterContextMenu.cs (R) 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterContextMenu: Form
{
     MenuItemColor micColor;

     public static void Main()
     {
          Application.Run(new BetterContextMenu());
     }
     public BetterContextMenu()
     {
          Text = "Better Context Menu Demo";

          EventHandler eh = new EventHandler(MenuColorOnClick);

          MenuItemColor[] amic = 
          {
               new MenuItemColor(Color.Black,   "&Black",  eh),
               new MenuItemColor(Color.Blue,    "B&lue",   eh),
               new MenuItemColor(Color.Green,   "&Green",   eh),
               new MenuItemColor(Color.Cyan,    "&Cyan",    eh),
               new MenuItemColor(Color.Red,     "&Red",     eh),
               new MenuItemColor(Color.Magenta, "&Magenta", eh),
               new MenuItemColor(Color.Yellow,  "&Yellow",  eh),
               new MenuItemColor(Color.White,   "&White",   eh)
          };

          foreach (MenuItemColor mic in amic)
               mic.RadioCheck = true;

          micColor = amic[3];
          micColor.Checked = true;
          BackColor = micColor.Color;

          ContextMenu = new ContextMenu(amic);
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
          micColor.Checked = false;
          micColor = (MenuItemColor) obj;
          micColor.Checked = true;

          BackColor = micColor.Color;
     }
}
class MenuItemColor: MenuItem
{
     Color clr;

     public MenuItemColor(Color clr, string str, EventHandler eh):
                                                            base(str, eh)
     {
          Color = clr;
     }
     public Color Color
     {
          get { return clr; }
          set { clr = value; }
     }
}