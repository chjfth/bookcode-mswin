//---------------------------------------
// ClipText.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ClipText: Form
{
     string   strText = "Sample text for the clipboard";
     MenuItem miCut, miCopy, miPaste;

     public static void Main()
     {
          Application.Run(new ClipText());
     }
     public ClipText()
     {
          Text = "Clip Text";
          ResizeRedraw = true;

          Menu = new MainMenu();

               // Edit menu

          MenuItem mi = new MenuItem("&Edit");
          mi.Popup += new EventHandler(MenuEditOnPopup);
          Menu.MenuItems.Add(mi);

               // Edit Cut menu item

          miCut = new MenuItem("Cu&t");
          miCut.Click += new EventHandler(MenuEditCutOnClick);
          miCut.Shortcut = Shortcut.CtrlX;
          Menu.MenuItems[0].MenuItems.Add(miCut);

               // Edit Copy menu item

          miCopy = new MenuItem("&Copy");
          miCopy.Click += new EventHandler(MenuEditCopyOnClick);
          miCopy.Shortcut = Shortcut.CtrlC;
          Menu.MenuItems[0].MenuItems.Add(miCopy);

               // Edit Paste menu item

          miPaste = new MenuItem("&Paste");
          miPaste.Click += new EventHandler(MenuEditPasteOnClick);
          miPaste.Shortcut = Shortcut.CtrlV;
          Menu.MenuItems[0].MenuItems.Add(miPaste);
     }
     void MenuEditOnPopup(object obj, EventArgs ea)
     {
          miCut.Enabled = 
          miCopy.Enabled = strText.Length > 0;
          miPaste.Enabled = 
                    Clipboard.GetDataObject().GetDataPresent(typeof(string));
     }
     void MenuEditCutOnClick(object obj, EventArgs ea)
     {
          MenuEditCopyOnClick(obj, ea);
          strText = "";
          Invalidate();
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          Clipboard.SetDataObject(strText, true);
     }
     void MenuEditPasteOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(typeof(string)))
               strText = (string) data.GetData(typeof(string));

          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString(strText, Font, new SolidBrush(ForeColor),
                          ClientRectangle, strfmt);
     }
}
