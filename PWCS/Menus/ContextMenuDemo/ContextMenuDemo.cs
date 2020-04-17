//----------------------------------------------
// ContextMenuDemo.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ContextMenuDemo: Form
{
     MenuItem miColor;

     public static void Main()
     {
          Application.Run(new ContextMenuDemo());
     }
     public ContextMenuDemo()
     {
          Text = "Context Menu Demo";

          EventHandler eh = new EventHandler(MenuColorOnClick);

          MenuItem[] ami = { new MenuItem("Black",   eh),
                             new MenuItem("Blue",    eh),
                             new MenuItem("Green",   eh),
                             new MenuItem("Cyan",    eh),
                             new MenuItem("Red",     eh),
                             new MenuItem("Magenta", eh),
                             new MenuItem("Yellow",  eh),
                             new MenuItem("White",   eh) };

          foreach (MenuItem mi in ami)
               mi.RadioCheck = true;

          miColor = ami[3];
          miColor.Checked = true;
          BackColor = Color.FromName(miColor.Text);

          ContextMenu = new ContextMenu(ami);
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
          miColor.Checked = false;
          miColor = (MenuItem) obj;
          miColor.Checked = true;

          BackColor = Color.FromName(miColor.Text);
     }
}
