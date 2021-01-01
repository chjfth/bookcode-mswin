//--------------------------------------------------
// DirectoriesAndFiles.cs (R) 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class DirectoriesAndFiles: Form
{
     DirectoryTreeView dirtree;
     Panel             panel;
     TreeNode          tnSelect;

     public static void Main()
     {
          Application.Run(new DirectoriesAndFiles());
     }
     public DirectoriesAndFiles()
     {
          Text = "Directories and Files";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          panel = new Panel();
          panel.Parent = this;
          panel.Dock = DockStyle.Fill;
          panel.Paint += new PaintEventHandler(PanelOnPaint);

          Splitter split = new Splitter();
          split.Parent = this;
          split.Dock = DockStyle.Left;
          split.BackColor = SystemColors.Control;

          dirtree = new DirectoryTreeView();
          dirtree.Parent = this;
          dirtree.Dock = DockStyle.Left;
          dirtree.AfterSelect += 
                    new TreeViewEventHandler(DirectoryTreeViewOnAfterSelect);

               // Create menu with one item.

          Menu = new MainMenu();
          Menu.MenuItems.Add("View");

          MenuItem mi = new MenuItem("Refresh", 
                              new EventHandler(MenuOnRefresh), Shortcut.F5);
          Menu.MenuItems[0].MenuItems.Add(mi);
     }
     void DirectoryTreeViewOnAfterSelect(object obj, TreeViewEventArgs tvea)
     {
          tnSelect = tvea.Node;
          panel.Invalidate();
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
     {
          if (tnSelect == null)
               return;

          Panel         panel     = (Panel) obj;
          Graphics      grfx      = pea.Graphics;
          DirectoryInfo dirinfo   = new DirectoryInfo(tnSelect.FullPath);
          FileInfo[]    afileinfo;
          Brush         brush     = new SolidBrush(panel.ForeColor);
          int           y         = 0;

          try
          {
               afileinfo = dirinfo.GetFiles();
          }
          catch
          {
               return;
          }

          foreach (FileInfo fileinfo in afileinfo)
          {
               grfx.DrawString(fileinfo.Name, Font, brush, 0, y);
               y += Font.Height;
          }
     }
     void MenuOnRefresh(object obj, EventArgs ea)
     {
          dirtree.RefreshTree();
     }
}
