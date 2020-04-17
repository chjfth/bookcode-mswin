//---------------------------------------------
// MetafileViewer.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Printing;
using System.IO;              // For Path class
using System.Windows.Forms;

class MetafileViewer: Form
{
     protected Metafile mf;
     protected string   strProgName;
     protected string   strFileName;
     MenuItem           miFileSaveAs, miFilePrint, 
                        miFileProps, miViewChecked;

     public static void Main()
     {
          Application.Run(new MetafileViewer());
     }
     public MetafileViewer()
     {
          Text = strProgName = "Metafile Viewer";
          ResizeRedraw = true;

          Menu = new MainMenu();

               // File menu

          MenuItem mi = new MenuItem("&File");
          mi.Popup += new EventHandler(MenuFileOnPopup);
          Menu.MenuItems.Add(mi);

               // File Open menu item

          mi = new MenuItem("&Open...");
          mi.Click += new EventHandler(MenuFileOpenOnClick);
          mi.Shortcut = Shortcut.CtrlO;
          Menu.MenuItems[0].MenuItems.Add(mi);

               // File Save As Bitmap menu item

          miFileSaveAs = new MenuItem("Save &As Bitmap...");
          miFileSaveAs.Click += new EventHandler(MenuFileSaveAsOnClick);
          Menu.MenuItems[0].MenuItems.Add(miFileSaveAs);
          Menu.MenuItems[0].MenuItems.Add("-");

               // File Print menu item

          miFilePrint = new MenuItem("&Print...");
          miFilePrint.Click += new EventHandler(MenuFilePrintOnClick);
          Menu.MenuItems[0].MenuItems.Add(miFilePrint);
          Menu.MenuItems[0].MenuItems.Add("-");

               // File Properties menu item

          miFileProps = new MenuItem("Propert&ies...");
          miFileProps.Click += new EventHandler(MenuFilePropsOnClick);
          Menu.MenuItems[0].MenuItems.Add(miFileProps);

               // Edit menu (temporary until Chapter 24)

          Menu.MenuItems.Add("&Edit");

               // View menu

          Menu.MenuItems.Add("&View");

          string[] astr = { "&Stretched to Window", 
                            "&Metrical Size", "&Pixel Size" };
          EventHandler eh = new EventHandler(MenuViewOnClick);

          foreach (string str in astr)
               Menu.MenuItems[2].MenuItems.Add(str, eh);

          miViewChecked = Menu.MenuItems[2].MenuItems[0];
          miViewChecked.Checked = true;
     }
     void MenuFileOnPopup(object obj, EventArgs ea)
     {
          miFileSaveAs.Enabled = 
          miFilePrint.Enabled =  
          miFileProps.Enabled = (mf != null);
     }
     void MenuFileOpenOnClick(object obj, EventArgs ea)
     {
          OpenFileDialog dlg = new OpenFileDialog();

          dlg.Filter = "All Metafiles|*.wmf;*.emf|" +
                       "Windows Metafile (*.wmf)|*.wmf|" +
                       "Enhanced Metafile (*.emf)|*.emf|" +
                       "All files|*.*";

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               try
               {
                    mf = new Metafile(dlg.FileName);
               }
               catch (Exception exc)
               {
                    MessageBox.Show(exc.Message, strProgName);
                    return;
               }
               strFileName = dlg.FileName;
               Text = strProgName + " - " + Path.GetFileName(strFileName);
               Invalidate();
          }
     }
     protected virtual void MenuFileSaveAsOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Not yet implemented!", strProgName);
     }
     void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
          PrintDocument prndoc = new PrintDocument();

          prndoc.DocumentName = Text;
          prndoc.PrintPage += new PrintPageEventHandler(OnPrintPage);
          prndoc.Print();
     }
     void MenuFilePropsOnClick(object obj, EventArgs ea)
     {
          MetafileHeader mh = mf.GetMetafileHeader();

          string str = 
               "Image Properties" +
               "\n\tSize = " + mf.Size + 
               "\n\tHorizontal Resolution = " + mf.HorizontalResolution +
               "\n\tVertical Resolution = " + mf.VerticalResolution +
               "\n\tPhysical Dimension = " + mf.PhysicalDimension +
               "\n\nMetafile Header Properties" +
               "\n\tBounds = " + mh.Bounds +
               "\n\tDpiX = " + mh.DpiX +
               "\n\tDpiY = " + mh.DpiY +
               "\n\tLogicalDpiX = " + mh.LogicalDpiX +
               "\n\tLogicalDpiY = " + mh.LogicalDpiY +
               "\n\tType = " + mh.Type +
               "\n\tVersion = " + mh.Version +
               "\n\tMetafileSize = " + mh.MetafileSize;

          MessageBox.Show(str, Text);
     }
     void MenuViewOnClick(object obj, EventArgs ea)
     {
          miViewChecked.Checked = false;
          miViewChecked = (MenuItem) obj;
          miViewChecked.Checked = true;
          Invalidate();
     }
     void OnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics  grfx = ppea.Graphics;
          Rectangle rect = new Rectangle(
               ppea.MarginBounds.Left -
               (ppea.PageBounds.Width - 
                    (int) grfx.VisibleClipBounds.Width) / 2,
               ppea.MarginBounds.Top -
               (ppea.PageBounds.Height - 
                    (int) grfx.VisibleClipBounds.Height) / 2,
               ppea.MarginBounds.Width,
               ppea.MarginBounds.Height);

          DisplayMetafile(grfx, rect);
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          if (mf != null)
               DisplayMetafile(pea.Graphics, ClientRectangle);
     }
     void DisplayMetafile(Graphics grfx, Rectangle rect)
     {
          switch (miViewChecked.Index)
          {
          case 0: grfx.DrawImage(mf, rect);  break;
          case 1: grfx.DrawImage(mf, rect.X, rect.Y);  break;
          case 2: grfx.DrawImage(mf, rect.X, rect.Y, mf.Width, mf.Height);
                  break;
          }
     }
}