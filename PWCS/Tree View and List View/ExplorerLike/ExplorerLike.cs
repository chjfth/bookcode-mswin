//-------------------------------------------
// ExplorerLike.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ExplorerLike: Form
{
     FileListView      filelist;
     DirectoryTreeView dirtree;
     MenuItemView      mivChecked;

     public static void Main()
     {
          Application.Run(new ExplorerLike());
     }
     public ExplorerLike()
     {
          Text = "Windows Explorer-Like Program";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create controls.

          filelist        = new FileListView();
          filelist.Parent = this;
          filelist.Dock   = DockStyle.Fill;

          Splitter split  = new Splitter();
          split.Parent    = this;
          split.Dock      = DockStyle.Left;
          split.BackColor = SystemColors.Control;

          dirtree        = new DirectoryTreeView();
          dirtree.Parent = this;
          dirtree.Dock   = DockStyle.Left;
          dirtree.AfterSelect += 
                    new TreeViewEventHandler(DirectoryTreeViewOnAfterSelect);

               // Create View menu.

          Menu = new MainMenu();
          Menu.MenuItems.Add("&View");

          string[] astrView = { "Lar&ge Icons", "S&mall Icons", 
                                "&List", "&Details" };
          View[] aview = { View.LargeIcon, View.SmallIcon, 
                           View.List, View.Details }; 
          EventHandler eh = new EventHandler(MenuOnView);

          for (int i = 0; i < 4; i++)
          {
               MenuItemView miv = new MenuItemView();
               miv.Text = astrView[i];
               miv.View = aview[i];
               miv.RadioCheck = true;
               miv.Click += eh;

               if (i == 3)         // Default == View.Details
               {
                    mivChecked = miv;
                    mivChecked.Checked = true;
                    filelist.View = mivChecked.View;
               }
               Menu.MenuItems[0].MenuItems.Add(miv);
          }
          Menu.MenuItems[0].MenuItems.Add("-");

               // View Refresh menu item

          MenuItem mi = new MenuItem("&Refresh", 
                              new EventHandler(MenuOnRefresh), Shortcut.F5);
          Menu.MenuItems[0].MenuItems.Add(mi);
     }
     void DirectoryTreeViewOnAfterSelect(object obj, TreeViewEventArgs tvea)
     {
          filelist.ShowFiles(tvea.Node.FullPath);
     }
     void MenuOnView(object obj, EventArgs ea)
     {
          mivChecked.Checked = false;
          mivChecked = (MenuItemView) obj;
          mivChecked.Checked = true;

          filelist.View = mivChecked.View;
     }
     void MenuOnRefresh(object obj, EventArgs ea)
     {
          dirtree.RefreshTree();
     }
}
class MenuItemView: MenuItem
{
     public View View;
}