//-------------------------------------------------
// TextBoxWithToolBar.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TextBoxWithToolBar: Form
{
     TextBox       txtbox;
     MenuItem      miEditCut, miEditCopy, miEditPaste;
     ToolBarButton tbbCut, tbbCopy, tbbPaste;

     public static void Main()
     {
               // Program doesn't run without this statement.

          System.Threading.Thread.CurrentThread.ApartmentState =
                                        System.Threading.ApartmentState.STA;

          Application.Run(new TextBoxWithToolBar());
     }
     public TextBoxWithToolBar()
     {
          Text = "Text Box with Toolbar";

               // Create TextBox.

          txtbox             = new TextBox();
          txtbox.Parent      = this;
          txtbox.Dock        = DockStyle.Fill;
          txtbox.Multiline   = true;
          txtbox.ScrollBars  = ScrollBars.Both;
          txtbox.AcceptsTab  = true;

               // Create ImageList.

          Bitmap bm = new Bitmap(GetType(), 
                                 "TextBoxWithToolBar.StandardButtons.bmp");

          ImageList imglst = new ImageList();
          imglst.Images.AddStrip(bm);
          imglst.TransparentColor = Color.Cyan;

               // Create ToolBar with ButtonClick event handler.

          ToolBar tbar = new ToolBar();
          tbar.Parent = this;
          tbar.ImageList = imglst;
          tbar.ShowToolTips = true;
          tbar.ButtonClick += 
                         new ToolBarButtonClickEventHandler(ToolBarOnClick);

               // Create the Edit menu.

          Menu = new MainMenu();

          MenuItem mi = new MenuItem("&Edit");
          mi.Popup += new EventHandler(MenuEditOnPopup);
          Menu.MenuItems.Add(mi);

               // Create the Edit Cut menu item.

          miEditCut = new MenuItem("Cu&t");
          miEditCut.Click += new EventHandler(MenuEditCutOnClick);
          miEditCut.Shortcut = Shortcut.CtrlX;
          Menu.MenuItems[0].MenuItems.Add(miEditCut);

               // And create the Cut toolbar button.

          tbbCut = new ToolBarButton();
          tbbCut.ImageIndex = 4;
          tbbCut.ToolTipText = "Cut";
          tbbCut.Tag = miEditCut;
          tbar.Buttons.Add(tbbCut);

               // Create the Edit Copy menu item.

          miEditCopy = new MenuItem("&Copy");
          miEditCopy.Click += new EventHandler(MenuEditCopyOnClick);
          miEditCopy.Shortcut = Shortcut.CtrlC;
          Menu.MenuItems[0].MenuItems.Add(miEditCopy);

               // And create the Copy toolbar button.

          tbbCopy = new ToolBarButton();
          tbbCopy.ImageIndex = 5;
          tbbCopy.ToolTipText = "Copy";
          tbbCopy.Tag = miEditCopy;
          tbar.Buttons.Add(tbbCopy);

               // Create the Edit Paste menu item.

          miEditPaste = new MenuItem("&Paste");
          miEditPaste.Click += new EventHandler(MenuEditPasteOnClick);
          miEditPaste.Shortcut = Shortcut.CtrlV;
          Menu.MenuItems[0].MenuItems.Add(miEditPaste);

               // And create the Paste toolbar button.

          tbbPaste = new ToolBarButton();
          tbbPaste.ImageIndex = 6;
          tbbPaste.ToolTipText = "Paste";
          tbbPaste.Tag = miEditPaste;
          tbar.Buttons.Add(tbbPaste);

               // Set Timer for enabling buttons.

          Timer timer = new Timer();
          timer.Interval = 250;
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Start();
     }
     void MenuEditOnPopup(object obj, EventArgs ea)
     {
          miEditCut.Enabled = 
          miEditCopy.Enabled = (txtbox.SelectionLength > 0);
          miEditPaste.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(typeof(string));
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          tbbCut.Enabled =
          tbbCopy.Enabled = (txtbox.SelectionLength) > 0;
          tbbPaste.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(typeof(string));
     }
     void ToolBarOnClick(object obj, ToolBarButtonClickEventArgs tbbcea)
     {
          ToolBarButton tbb = tbbcea.Button;
          MenuItem mi = (MenuItem) tbb.Tag;

          mi.PerformClick();
     }
     void MenuEditCutOnClick(object obj, EventArgs ea)
     {
          txtbox.Cut();
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          txtbox.Copy();
     }
     void MenuEditPasteOnClick(object obj, EventArgs ea)
     {
          txtbox.Paste();
     }
}          
