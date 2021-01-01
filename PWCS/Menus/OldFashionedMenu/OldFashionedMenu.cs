//-----------------------------------------------
// OldFashionedMenu.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class OldFashionedMenu: Form
{
     MainMenu mmMain, mmFile, mmEdit;

     public static void Main()
     {
          Application.Run(new OldFashionedMenu());
     }
     public OldFashionedMenu()
     {
          Text = "Old-Fashioned Menu";

          EventHandler eh = new EventHandler(MenuOnClick);

          mmMain = new MainMenu(new MenuItem[] 
          { 
               new MenuItem("MAIN:"),
               new MenuItem("&File", new EventHandler(MenuFileOnClick)),
               new MenuItem("&Edit", new EventHandler(MenuEditOnClick))
          });

          mmFile = new MainMenu(new MenuItem[]
          {
               new MenuItem("FILE:"),
               new MenuItem("&New", eh),
               new MenuItem("&Open...", eh),
               new MenuItem("&Save", eh),
               new MenuItem("Save &As...", eh),
               new MenuItem("(&Main)", new EventHandler(MenuMainOnClick))
          });
          
          mmEdit = new MainMenu(new MenuItem[]
          {
               new MenuItem("EDIT:"),
               new MenuItem("Cu&t", eh),
               new MenuItem("&Copy", eh),
               new MenuItem("&Paste", eh),
               new MenuItem("De&lete", eh),
               new MenuItem("(&Main)", new EventHandler(MenuMainOnClick))
          });

          Menu = mmMain;
     }
     void MenuMainOnClick(object obj, EventArgs ea)
     {
          Menu = mmMain;
     }
     void MenuFileOnClick(object obj, EventArgs ea)
     {
          Menu = mmFile;
     }
     void MenuEditOnClick(object obj, EventArgs ea)
     {
          Menu = mmEdit;
     }
     void MenuOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Menu item clicked!", Text);
     }
}