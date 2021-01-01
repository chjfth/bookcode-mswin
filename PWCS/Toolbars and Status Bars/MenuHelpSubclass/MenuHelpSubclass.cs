//-----------------------------------------------
// MenuHelpSubclass.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MenuHelpSubclass: Form
{
     StatusBarPanel sbpMenuHelp;
     string         strSavePanelText;

     public static void Main()
     {
          Application.Run(new MenuHelpSubclass());
     }
     public MenuHelpSubclass()
     {
          Text = "Menu Help";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create a status bar with one panel.

          StatusBar sb = new StatusBar();
          sb.Parent = this;
          sb.ShowPanels = true;

          sbpMenuHelp = new StatusBarPanel();
          sbpMenuHelp.Text = "Ready";
          sbpMenuHelp.AutoSize = StatusBarPanelAutoSize.Spring;

          sb.Panels.Add(sbpMenuHelp);

               // Construct a simple menu with MenuItemHelp items.

          Menu = new MainMenu();
          
               // Create File menu items.

          MenuItemHelp mi = new MenuItemHelp("&File");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Commands for working with files";
          Menu.MenuItems.Add(mi);

          mi = new MenuItemHelp("&Open...");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Opens an existing document";
          Menu.MenuItems[0].MenuItems.Add(mi);
          
          mi = new MenuItemHelp("&Close");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Closes the current document";
          Menu.MenuItems[0].MenuItems.Add(mi);

          mi = new MenuItemHelp("&Save");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Saves the current document";
          Menu.MenuItems[0].MenuItems.Add(mi);

               // Create Edit menu items.

          mi = new MenuItemHelp("&Edit");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Commands for editing the document";
          Menu.MenuItems.Add(mi);

          mi = new MenuItemHelp("Cu&t");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Deletes the selection and " +
                        "copies it to the clipboard";
          Menu.MenuItems[1].MenuItems.Add(mi);
          
          mi = new MenuItemHelp("&Copy");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Copies the selection to the clipboard";
          Menu.MenuItems[1].MenuItems.Add(mi);

          mi = new MenuItemHelp("&Paste");
          mi.HelpPanel = sbpMenuHelp;
          mi.HelpText = "Replaces the current selection " +
                        "with the clipboard contents";
          Menu.MenuItems[1].MenuItems.Add(mi);
     }
     protected override void OnMenuStart(EventArgs ea)
     {
          strSavePanelText = sbpMenuHelp.Text;
     }
     protected override void OnMenuComplete(EventArgs ea)
     {
          sbpMenuHelp.Text = strSavePanelText;
     }
}
