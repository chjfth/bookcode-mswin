//---------------------------------------------
// ImageDirectory.cs © 2001 by Charles Petzold
//---------------------------------------------
using Petzold.ProgrammingWindowsWithCSharp;
using System;
using System.Drawing;
using System.Windows.Forms;

class ImageDirectory: Form
{
     PictureBoxPlus    picbox;
     DirectoryTreeView dirtree;
     ImagePanel        imgpanel;
     Splitter          split;
     TreeNode          tnSelect;
     Control           cntlClicked;
     Point             ptPanelAutoScroll;

     public static void Main()
     {
          Application.Run(new ImageDirectory());
     }
     public ImageDirectory()
     {
          Text = "Image Directory";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

               // Create (invisible) control for displaying large image.

          picbox            = new PictureBoxPlus();
          picbox.Parent     = this;
          picbox.Visible    = false;
          picbox.Dock       = DockStyle.Fill;
          picbox.SizeMode   = PictureBoxSizeMode.StretchImage;
          picbox.NoDistort  = true;
          picbox.MouseDown += new MouseEventHandler(PictureBoxOnMouseDown);

               // Create controls for displaying thumbnails.

          imgpanel        = new ImagePanel();
          imgpanel.Parent = this;
          imgpanel.Dock   = DockStyle.Fill;
          imgpanel.ImageClicked += 
                         new EventHandler(ImagePanelOnImageClicked);

          split           = new Splitter();
          split.Parent    = this;
          split.Dock      = DockStyle.Left;
          split.BackColor = SystemColors.Control;

          dirtree        = new DirectoryTreeView();
          dirtree.Parent = this;
          dirtree.Dock   = DockStyle.Left;
          dirtree.AfterSelect += 
                    new TreeViewEventHandler(DirectoryTreeViewOnAfterSelect);

               // Create menu with one item (Refresh).

          Menu = new MainMenu();
          Menu.MenuItems.Add("&View");
          MenuItem mi = new MenuItem("&Refresh", 
                              new EventHandler(MenuOnRefresh), Shortcut.F5);
          Menu.MenuItems[0].MenuItems.Add(mi);
     }
     void DirectoryTreeViewOnAfterSelect(object obj, TreeViewEventArgs tvea)
     {
          tnSelect = tvea.Node;
          imgpanel.ShowImages(tnSelect.FullPath);
     }
     void MenuOnRefresh(object obj, EventArgs ea)
     {
          dirtree.RefreshTree();
     }
     void ImagePanelOnImageClicked(object obj, EventArgs ea)
     {
               // Get clicked control and image.
          
          cntlClicked = imgpanel.ClickedControl;
          picbox.Image = imgpanel.ClickedImage;

               // Save auto-scroll position.

          ptPanelAutoScroll = imgpanel.AutoScrollPosition;
          ptPanelAutoScroll.X *= -1;
          ptPanelAutoScroll.Y *= -1;

               // Hide and disable the normal controls.

          imgpanel.Visible = false;
          imgpanel.Enabled = false;
          imgpanel.AutoScrollPosition = Point.Empty;

          split.Visible = false;
          split.Enabled = false;

          dirtree.Visible = false;
          dirtree.Enabled = false;

               // Make the picture box visible.

          picbox.Visible = true;
     }
          // Event handlers and method involved with restoring controls

     void PictureBoxOnMouseDown(object obj, MouseEventArgs mea)
     {
          RestoreControls();
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          if (kea.KeyCode == Keys.Escape)
               RestoreControls();
     }
     void RestoreControls()
     {
          picbox.Visible = false;

          dirtree.Visible = true;
          dirtree.Enabled = true;

          split.Enabled = true;
          split.Visible = true;

          imgpanel.AutoScrollPosition = ptPanelAutoScroll;
          imgpanel.Visible = true;
          imgpanel.Enabled = true;
          cntlClicked.Focus();
     }
}
