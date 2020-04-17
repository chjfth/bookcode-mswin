//--------------------------------------------
// FirstMainMenu.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FirstMainMenu: Form
{
     public static void Main()
     {
          Application.Run(new FirstMainMenu());
     }
     public FirstMainMenu()
     {
          Text = "First Main Menu";

               // Items on File submenu

          MenuItem miOpen = new MenuItem("&Open...",
                                   new EventHandler(MenuFileOpenOnClick),
                                   Shortcut.CtrlO);

          MenuItem miSave = new MenuItem("&Save",
                                   new EventHandler(MenuFileSaveOnClick),
                                   Shortcut.CtrlS);

          MenuItem miSaveAs = new MenuItem("Save &As...",
                                   new EventHandler(MenuFileSaveAsOnClick));

          MenuItem miDash = new MenuItem("-");

          MenuItem miExit = new MenuItem("E&xit",
                                   new EventHandler(MenuFileExitOnClick));
               // File item

          MenuItem miFile = new MenuItem("&File",
                                   new MenuItem[] {miOpen, miSave, miSaveAs,
                                                   miDash, miExit });
               // Items on Edit submenu

          MenuItem miCut = new MenuItem("Cu&t",
                                   new EventHandler(MenuEditCutOnClick),
                                   Shortcut.CtrlX);

          MenuItem miCopy = new MenuItem("&Copy",
                                   new EventHandler(MenuEditCopyOnClick),
                                   Shortcut.CtrlC);

          MenuItem miPaste = new MenuItem("&Paste",
                                   new EventHandler(MenuEditPasteOnClick),
                                   Shortcut.CtrlV);
               // Edit item

          MenuItem miEdit = new MenuItem("&Edit",
                                   new MenuItem[] {miCut, miCopy, miPaste});

               // Item on Help submenu

          MenuItem miAbout = new MenuItem("&About FirstMainMenu...",
                                   new EventHandler(MenuHelpAboutOnClick));
               // Help item

          MenuItem miHelp = new MenuItem("&Help", 
                                   new MenuItem[] {miAbout});
               // Main menu

          Menu = new MainMenu(new MenuItem[] {miFile, miEdit, miHelp});
     }
     void MenuFileOpenOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("File Open item clicked!", Text);
     }
     void MenuFileSaveOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("File Save item clicked!", Text);
     }
     void MenuFileSaveAsOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("File Save As item clicked!", Text);
     }
     void MenuFileExitOnClick(object obj, EventArgs ea)
     {
          Close();
     }
     void MenuEditCutOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Edit Cut item clicked!", Text);
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Edit Copy item clicked!", Text);
     }
     void MenuEditPasteOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Edit Paste item clicked!", Text);
     }
     void MenuHelpAboutOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show(Text + " © 2001 by Charles Petzold");
     }
}