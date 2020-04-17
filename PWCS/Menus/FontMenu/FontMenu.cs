//---------------------------------------
// FontMenu.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FontMenu: Form
{
     const int iPointSize = 24;
     string    strFacename;

     public static void Main()
     {
          Application.Run(new FontMenu());
     }
     public FontMenu()
     {
          Text = "Font Menu";

          strFacename = Font.Name;

          Menu = new MainMenu();

          MenuItem mi = new MenuItem("&Facename");
          mi.Popup += new EventHandler(MenuFacenameOnPopup);
          mi.MenuItems.Add(" ");   // Necessary for pop-up call
          Menu.MenuItems.Add(mi);
     }
     void MenuFacenameOnPopup(object obj, EventArgs ea)
     {
          MenuItem     miFacename = (MenuItem)obj;
          FontFamily[] aff        = FontFamily.Families;
          EventHandler ehClick    = new EventHandler(MenuFacenameOnClick);
          MenuItem[]   ami        = new MenuItem[aff.Length];

          for (int i = 0; i < aff.Length; i++)
          {
               ami[i] = new MenuItem(aff[i].Name);
               ami[i].Click += ehClick;

               if (aff[i].Name == strFacename)
                    ami[i].Checked = true;
          }
          miFacename.MenuItems.Clear();
          miFacename.MenuItems.AddRange(ami);
     }
     void MenuFacenameOnClick(object obj, EventArgs ea)
     {    
          MenuItem mi = (MenuItem)obj;
          strFacename = mi.Text;
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Font     font = new Font(strFacename, iPointSize);

          StringFormat strfmt  = new StringFormat();
          strfmt.Alignment     = StringAlignment.Center;
          strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString("Sample Text", font, new SolidBrush(ForeColor), 
                          ClientRectangle, strfmt);
     }                        
}