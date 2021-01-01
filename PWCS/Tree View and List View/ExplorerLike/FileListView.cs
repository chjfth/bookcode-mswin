//-------------------------------------------
// FileListView.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Diagnostics;          // For Process.Start
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class FileListView: ListView
{
     string strDirectory;

     public FileListView()
     {
          View = View.Details;

               // Get images for file icons.

          ImageList imglst = new ImageList();
          imglst.Images.Add(new Bitmap(GetType(), "DOC.BMP"));
          imglst.Images.Add(new Bitmap(GetType(), "EXE.BMP"));

          SmallImageList = imglst;
          LargeImageList = imglst;

               // Create columns.

          Columns.Add("Name",      100, HorizontalAlignment.Left);
          Columns.Add("Size",      100, HorizontalAlignment.Right);
          Columns.Add("Modified",  100, HorizontalAlignment.Left);
          Columns.Add("Attribute", 100, HorizontalAlignment.Left);
     }
     public void ShowFiles(string strDirectory)
     {
               // Save directory name as field.
               
          this.strDirectory = strDirectory;

          Items.Clear();
          DirectoryInfo dirinfo = new DirectoryInfo(strDirectory);
          FileInfo[] afileinfo;

          try
          {
               afileinfo = dirinfo.GetFiles();
          }
          catch
          {
               return;
          }

          foreach (FileInfo fi in afileinfo)
          {
                    // Create ListViewItem.

               ListViewItem lvi = new ListViewItem(fi.Name);

                    // Assign ImageIndex based on filename extension.

               if (Path.GetExtension(fi.Name).ToUpper() == ".EXE")
                    lvi.ImageIndex = 1;
               else
                    lvi.ImageIndex = 0;

                    // Add file length and modified time sub-items.

               lvi.SubItems.Add(fi.Length.ToString("N0"));
               lvi.SubItems.Add(fi.LastWriteTime.ToString());

                    // Add attribute subitem.

               string strAttr = "";
               
               if ((fi.Attributes & FileAttributes.Archive) != 0)
                    strAttr += "A";

               if ((fi.Attributes & FileAttributes.Hidden) != 0)
                    strAttr += "H";

               if ((fi.Attributes & FileAttributes.ReadOnly) != 0)
                    strAttr += "R";

               if ((fi.Attributes & FileAttributes.System) != 0)
                    strAttr += "S";

               lvi.SubItems.Add(strAttr);

                    // Add completed ListViewItem to FileListView.

               Items.Add(lvi);
          }
     }
     protected override void OnItemActivate(EventArgs ea)
     {
          base.OnItemActivate(ea);

          foreach (ListViewItem lvi in SelectedItems)
          {
               try
               {
                    Process.Start(Path.Combine(strDirectory, lvi.Text));
               }
               catch
               {
                    continue;
               }
          }
     }
}
