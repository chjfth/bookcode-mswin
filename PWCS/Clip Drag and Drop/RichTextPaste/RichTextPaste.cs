//--------------------------------------------
// RichTextPaste.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class RichTextPaste: Form
{
     string   strPastedText = "";
     MenuItem miPastePlain, miPasteRTF, miPasteHTML, miPasteCSV;

     public static void Main()
     {
          Application.Run(new RichTextPaste());
     }
     public RichTextPaste()
     {
          Text = "Rich-Text Paste";
          ResizeRedraw = true;

          Menu = new MainMenu();

               // Edit menu

          MenuItem mi = new MenuItem("&Edit");
          mi.Popup += new EventHandler(MenuEditOnPopup);
          Menu.MenuItems.Add(mi);

               // Edit Paste Plain Text menu item

          miPastePlain = new MenuItem("Paste &Plain Text");
          miPastePlain.Click += new EventHandler(MenuEditPastePlainOnClick);
          Menu.MenuItems[0].MenuItems.Add(miPastePlain);

               // Edit Paste RTF menu item

          miPasteRTF = new MenuItem("Paste &Rich Text Format");
          miPasteRTF.Click += new EventHandler(MenuEditPasteRTFOnClick);
          Menu.MenuItems[0].MenuItems.Add(miPasteRTF);

               // Edit Paste HTML menu item

          miPasteHTML = new MenuItem("Paste &HTML");
          miPasteHTML.Click += new EventHandler(MenuEditPasteHTMLOnClick);
          Menu.MenuItems[0].MenuItems.Add(miPasteHTML);

               // Edit Paste CSV menu item

          miPasteCSV = new MenuItem("Paste &Comma-Separated Values");
          miPasteCSV.Click += new EventHandler(MenuEditPasteCSVOnClick);
          Menu.MenuItems[0].MenuItems.Add(miPasteCSV);
     }
     void MenuEditOnPopup(object obj, EventArgs ea)
     {
          miPastePlain.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(typeof(string));
          miPasteRTF.Enabled =
               Clipboard.GetDataObject().GetDataPresent(DataFormats.Rtf);
          miPasteHTML.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(DataFormats.Html);
          miPasteCSV.Enabled = 
               Clipboard.GetDataObject().GetDataPresent
                                        (DataFormats.CommaSeparatedValue);
     }
     void MenuEditPastePlainOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(typeof(string)))
          {
               strPastedText = (string) data.GetData(typeof(string));
               Invalidate();
          }
     }
     void MenuEditPasteRTFOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(DataFormats.Rtf))
          {
               strPastedText = (string) data.GetData(DataFormats.Rtf);
               Invalidate();
          }
     }
     void MenuEditPasteHTMLOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(DataFormats.Html))
          {
               strPastedText = (string) data.GetData(DataFormats.Html);
               Invalidate();
          }
     }
     void MenuEditPasteCSVOnClick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          if (data.GetDataPresent(DataFormats.CommaSeparatedValue))
          {
               MemoryStream memstr = (MemoryStream) data.GetData("Csv");
               StreamReader streamreader = new StreamReader(memstr);
               strPastedText = streamreader.ReadToEnd();
               Invalidate();
          }
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.DrawString(strPastedText, Font, new SolidBrush(ForeColor),
                          ClientRectangle);
     }
}
