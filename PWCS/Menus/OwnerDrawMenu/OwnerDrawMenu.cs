//--------------------------------------------
// OwnerDrawMenu.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Text;    // For HotkeyPrefix enumeration
using System.Windows.Forms;

class OwnerDrawMenu: Form
{
     const    int  iFontPointSize = 18; // For menu items
     MenuItem      miFacename;

     public static void Main()
     {
          Application.Run(new OwnerDrawMenu());
     }
     public OwnerDrawMenu()
     {
          Text = "Owner-Draw Menu";

               // Top-level items
          
          Menu = new MainMenu();
          Menu.MenuItems.Add("&Facename");

               // Array of items on submenu

          string[] astrText = {"&Times New Roman", "&Arial", "&Courier New"};
          MenuItem [] ami = new MenuItem[astrText.Length];

          EventHandler ehOnClick = new EventHandler(MenuFacenameOnClick);
          MeasureItemEventHandler ehOnMeasureItem = 
                    new MeasureItemEventHandler(MenuFacenameOnMeasureItem);
          DrawItemEventHandler ehOnDrawItem = 
                    new DrawItemEventHandler(MenuFacenameOnDrawItem);

          for (int i = 0; i < ami.Length; i++)
          {
               ami[i]              = new MenuItem(astrText[i]);
               ami[i].OwnerDraw    = true;
               ami[i].RadioCheck   = true;
               ami[i].Click       += ehOnClick;
               ami[i].MeasureItem += ehOnMeasureItem;
               ami[i].DrawItem    += ehOnDrawItem;
          }
          miFacename = ami[0];
          miFacename.Checked = true;

          Menu.MenuItems[0].MenuItems.AddRange(ami);
     }
     void MenuFacenameOnClick(object obj, EventArgs ea)
     {
          miFacename.Checked = false;
          miFacename = (MenuItem) obj;
          miFacename.Checked = true;

          Invalidate();
     }
     void MenuFacenameOnMeasureItem(object obj, MeasureItemEventArgs miea)
     {
          MenuItem mi = (MenuItem) obj;
          Font font = new Font(mi.Text.Substring(1), iFontPointSize);

          StringFormat strfmt = new StringFormat();
          strfmt.HotkeyPrefix = HotkeyPrefix.Show;

          SizeF sizef = miea.Graphics.MeasureString(mi.Text, font, 
                                                    1000, strfmt);

          miea.ItemWidth  = (int) Math.Ceiling(sizef.Width);
          miea.ItemHeight = (int) Math.Ceiling(sizef.Height);

          miea.ItemWidth += SystemInformation.MenuCheckSize.Width * 
                              miea.ItemHeight / 
                                   SystemInformation.MenuCheckSize.Height;

          miea.ItemWidth -= SystemInformation.MenuCheckSize.Width;
     }
     void MenuFacenameOnDrawItem(object obj, DrawItemEventArgs diea) 
     {
          MenuItem mi   = (MenuItem) obj;
          Graphics grfx = diea.Graphics;
          Brush    brush;

               // Create the Font and StringFormat.

          Font font = new Font(mi.Text.Substring(1), iFontPointSize);
          StringFormat strfmt = new StringFormat();
          strfmt.HotkeyPrefix = HotkeyPrefix.Show;

               // Calculate check mark and text rectangles.

          Rectangle rectCheck = diea.Bounds;

          rectCheck.Width = SystemInformation.MenuCheckSize.Width * 
                              rectCheck.Height / 
                                   SystemInformation.MenuCheckSize.Height;

          Rectangle rectText = diea.Bounds;

          rectText.X += rectCheck.Width;

               // Do all the drawing.

          diea.DrawBackground();

          if ((diea.State & DrawItemState.Checked) != 0)
               ControlPaint.DrawMenuGlyph(grfx, rectCheck, MenuGlyph.Bullet);
          
          if ((diea.State & DrawItemState.Selected) != 0)
               brush = SystemBrushes.HighlightText;
          else
               brush = SystemBrushes.FromSystemColor(SystemColors.MenuText);

          grfx.DrawString(mi.Text, font, brush, rectText, strfmt);
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Font font = new Font(miFacename.Text.Substring(1), 12);
          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString(Text, font, new SolidBrush(ForeColor), 0, 0);
     }
}
