//-----------------------------------------------
// MenuHelpFirstTry.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MenuHelpFirstTry: Form
{
     StatusBarPanel sbpMenuHelp;
     string         strSavePanelText;

     public static void Main()
     {
          Application.Run(new MenuHelpFirstTry());
     }
     public MenuHelpFirstTry()
     {
          Text = "Menu Help (First Try)";
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

               // Construct a simple menu.
               // For this demo, we can ignore the Click handlers
               //   but what we really need are Select handlers.

          Menu = new MainMenu();
          EventHandler ehSelect = new EventHandler(MenuOnSelect);
          
               // Create File menu items.

          MenuItem mi = new MenuItem("File");
          mi.Select += ehSelect;
          Menu.MenuItems.Add(mi);

          mi = new MenuItem("Open");
          mi.Select += ehSelect;
          Menu.MenuItems[0].MenuItems.Add(mi);
          
          mi = new MenuItem("Close");
          mi.Select += ehSelect;
          Menu.MenuItems[0].MenuItems.Add(mi);

          mi = new MenuItem("Save");
          mi.Select += ehSelect;
          Menu.MenuItems[0].MenuItems.Add(mi);

               // Create Edit menu items.

          mi = new MenuItem("Edit");
          mi.Select += ehSelect;
          Menu.MenuItems.Add(mi);

          mi = new MenuItem("Cut");
          mi.Select += ehSelect;
          Menu.MenuItems[1].MenuItems.Add(mi);
          
          mi = new MenuItem("Copy");
          mi.Select += ehSelect;
          Menu.MenuItems[1].MenuItems.Add(mi);

          mi = new MenuItem("Paste");
          mi.Select += ehSelect;
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
     void MenuOnSelect(object obj, EventArgs ea)
     {
          MenuItem mi = (MenuItem) obj;
          string   str;

          switch (mi.Text)
          {
          case "File":   str = "Commands for working with files";    break;
          case "Open":   str = "Opens an existing document";         break;
          case "Close":  str = "Closes the current document";        break;
          case "Save":   str = "Saves the current document";         break;
          case "Edit":   str = "Commands for editing the document";  break;
          case "Cut":    str = "Deletes the selection and " +
                               "copies it to the clipboard";         break;
          case "Copy":   str = "Copies the selection to the " +
                               "clipboard";                          break;
          case "Paste":  str = "Replaces the current selection " +
                               "with the clipboard contents";        break;
          default:       str = "";                                   break;
          }

          sbpMenuHelp.Text = str;
     }
}
