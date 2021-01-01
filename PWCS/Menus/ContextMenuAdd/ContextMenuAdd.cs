//---------------------------------------------
// ContextMenuAdd.cs (R) 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ContextMenuAdd: Form
{
     MenuItem miColor;

     public static void Main()
     {
          Application.Run(new ContextMenuAdd());
     }
     public ContextMenuAdd()
     {
          Text = "Context Menu Using Add";

          ContextMenu  cm = new ContextMenu();
          EventHandler eh = new EventHandler(MenuColorOnClick);

          cm.MenuItems.Add("Black",   eh);
          cm.MenuItems.Add("Blue",    eh);
          cm.MenuItems.Add("Green",   eh);
          cm.MenuItems.Add("Cyan",    eh);
          cm.MenuItems.Add("Red",     eh);
          cm.MenuItems.Add("Magenta", eh);
          cm.MenuItems.Add("Yellow",  eh);
          cm.MenuItems.Add("White",   eh);

          foreach (MenuItem mi in cm.MenuItems)
               mi.RadioCheck = true;

          miColor = cm.MenuItems[3];
          miColor.Checked = true;
          BackColor = Color.FromName(miColor.Text);

          ContextMenu = cm;
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
          miColor.Checked = false;
          miColor = (MenuItem) obj;
          miColor.Checked = true;

          BackColor = Color.FromName(miColor.Text);
     }
}
