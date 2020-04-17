//---------------------------------------------
// DirectoryTreeView.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class DirectoryTreeView: TreeView
{
     public DirectoryTreeView()
     {
               // Make a little more room for long directory names.

          Width *= 2;

               // Get images for tree.

          ImageList = new ImageList();
          ImageList.Images.Add(new Bitmap(GetType(), "35FLOPPY.BMP"));
          ImageList.Images.Add(new Bitmap(GetType(), "CLSDFOLD.BMP"));
          ImageList.Images.Add(new Bitmap(GetType(), "OPENFOLD.BMP"));

               // Construct tree.

          RefreshTree();
     }
     public void RefreshTree()
     {
               // Turn off visual updating and clear tree.

          BeginUpdate();
          Nodes.Clear();

               // Make disk drives the root nodes. 

          string[] astrDrives = Directory.GetLogicalDrives();

          foreach (string str in astrDrives)
          {
               TreeNode tnDrive = new TreeNode(str, 0, 0);
               Nodes.Add(tnDrive);
               AddDirectories(tnDrive);

               if (str == "C:\\")
                    SelectedNode = tnDrive;
          }
          EndUpdate();
     }
     void AddDirectories(TreeNode tn)
     {
          tn.Nodes.Clear();

          string          strPath = tn.FullPath;
          DirectoryInfo   dirinfo = new DirectoryInfo(strPath);
          DirectoryInfo[] adirinfo;

          try
          {
               adirinfo = dirinfo.GetDirectories();
          }
          catch
          {
               return;
          }

          foreach (DirectoryInfo di in adirinfo)
          {
               TreeNode tnDir = new TreeNode(di.Name, 1, 2);
               tn.Nodes.Add(tnDir);

               // We could now fill up the whole tree with this statement:
               //        AddDirectories(tnDir);
               // But it would be too slow. Try it!
          }
     }
     protected override void OnBeforeExpand(TreeViewCancelEventArgs tvcea)
     {
          base.OnBeforeExpand(tvcea);

          BeginUpdate();

          foreach (TreeNode tn in tvcea.Node.Nodes)
               AddDirectories(tn);

          EndUpdate();
     }
}