//-------------------------------------------
// MetafileClip.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;

class MetafileClip: MetafileConvert
{
     MenuItem miCut, miCopy, miPaste, miDel;

     public new static void Main()
     {
          Application.Run(new MetafileClip());
     }
     public MetafileClip()
     {
          Text = strProgName = "Metafile Clip";

               // Edit menu

          Menu.MenuItems[1].Popup += new EventHandler(MenuEditOnPopup);

               // Edit Cut menu item

          miCut = new MenuItem("Cu&t");
          miCut.Click += new EventHandler(MenuEditCutOnClick);
          miCut.Shortcut = Shortcut.CtrlX;
          Menu.MenuItems[1].MenuItems.Add(miCut);

               // Edit Copy menu item

          miCopy = new MenuItem("&Copy");
          miCopy.Click += new EventHandler(MenuEditCopyOnClick);
          miCopy.Shortcut = Shortcut.CtrlC;
          Menu.MenuItems[1].MenuItems.Add(miCopy);

               // Edit Paste menu item

          miPaste = new MenuItem("&Paste");
          miPaste.Click += new EventHandler(MenuEditPasteOnClick);
          miPaste.Shortcut = Shortcut.CtrlV;
          Menu.MenuItems[1].MenuItems.Add(miPaste);

               // Edit Delete menu item

          miDel = new MenuItem("De&lete");
          miDel.Click += new EventHandler(MenuEditDelOnClick);
          miDel.Shortcut = Shortcut.Del;
          Menu.MenuItems[1].MenuItems.Add(miDel);
     }
     void MenuEditOnPopup(object obj, EventArgs ea)
     {
          miCut.Enabled = 
          miCopy.Enabled = 
          miDel.Enabled = mf != null;
          miPaste.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(typeof(Metafile));
     }
     void MenuEditCutOnClick(object obj, EventArgs ea)
     {
          MenuEditCopyOnClick(obj, ea);
          MenuEditDelOnClick(obj, ea);
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          Clipboard.SetDataObject(mf, true);
     }
     void MenuEditPasteOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(typeof(Metafile)))
               mf = (Metafile) data.GetData(typeof(Metafile));

          strFileName = "clipboard";
          Text = strProgName + " - " + strFileName;
          Invalidate();
     }
     void MenuEditDelOnClick(object obj, EventArgs ea)
     {
          mf = null;
          strFileName = null;
          Text = strProgName;
          Invalidate();
     }
}
