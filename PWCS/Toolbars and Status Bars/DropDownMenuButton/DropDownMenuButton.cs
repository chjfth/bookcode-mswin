//-------------------------------------------------
// DropDownMenuButton.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DropDownMenuButton: ToggleButtons
{
     public new static void Main()
     {
          Application.Run(new DropDownMenuButton());
     }
     public DropDownMenuButton()
     {
          Text = "Drop-Down Menu Button";
          strText = "Drop-Down";

               // Create bitmap for new button and add it to ImageList.

          tbar.ImageList.Images.Add(CreateBitmapButton(clrText));

               // Create the menu for the button.

          ContextMenu menu = new ContextMenu();

          EventHandler ehOnClick = new EventHandler(MenuColorOnClick);
          MeasureItemEventHandler ehOnMeasureItem = 
                    new MeasureItemEventHandler(MenuColorOnMeasureItem);
          DrawItemEventHandler ehOnDrawItem =
                    new DrawItemEventHandler(MenuColorOnDrawItem);

          Color[] acolor = 
          { 
          Color.FromArgb(0x00, 0x00, 0x00), Color.FromArgb(0x00, 0x00, 0x80),
          Color.FromArgb(0x00, 0x80, 0x00), Color.FromArgb(0x00, 0x80, 0x80),
          Color.FromArgb(0x80, 0x00, 0x00), Color.FromArgb(0x80, 0x00, 0x80),
          Color.FromArgb(0x80, 0x80, 0x00), Color.FromArgb(0x80, 0x80, 0x80),
          Color.FromArgb(0xC0, 0xC0, 0xC0), Color.FromArgb(0x00, 0x00, 0xFF),
          Color.FromArgb(0x00, 0xFF, 0x00), Color.FromArgb(0x00, 0xFF, 0xFF),
          Color.FromArgb(0xFF, 0x00, 0x00), Color.FromArgb(0xFF, 0x00, 0xFF),
          Color.FromArgb(0xFF, 0xFF, 0x00), Color.FromArgb(0xFF, 0xFF, 0xFF)
          };

          for (int i = 0; i < acolor.Length; i++)
          {
               MenuItemColor mic = new MenuItemColor();
               mic.OwnerDraw     = true;
               mic.Color         = acolor[i];
               mic.Click        += ehOnClick;
               mic.MeasureItem  += ehOnMeasureItem;
               mic.DrawItem     += ehOnDrawItem;
               mic.Break         = i % 4 == 0;

               menu.MenuItems.Add(mic);
          }
                // Finally, make the button itself.

          ToolBarButton tbarbtn = new ToolBarButton();
          tbarbtn.ImageIndex = 4;
          tbarbtn.Style = ToolBarButtonStyle.DropDownButton;
          tbarbtn.DropDownMenu = menu;
          tbarbtn.ToolTipText = "Color";

          tbar.Buttons.Add(tbarbtn);
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
               // Set the new text color.

          MenuItemColor mic = (MenuItemColor) obj;
          clrText = mic.Color;
          panel.Invalidate();

               // Make a new button bitmap.

          tbar.ImageList.Images[4] = CreateBitmapButton(clrText);
          tbar.Invalidate();
     }
     void MenuColorOnMeasureItem(object obj, MeasureItemEventArgs miea)
     {
          miea.ItemHeight = 18;
          miea.ItemWidth = 18;
     }
     void MenuColorOnDrawItem(object obj, DrawItemEventArgs diea)
     {
          MenuItemColor mic = (MenuItemColor) obj;
          Brush brush = new SolidBrush(mic.Color);

          Rectangle rect = diea.Bounds;

          rect.X += 1;
          rect.Y += 1;
          rect.Width -= 2;
          rect.Height -= 2;

          diea.Graphics.FillRectangle(brush, rect);
     }
     Bitmap CreateBitmapButton(Color clr)
     {
          Bitmap   bm     = new Bitmap(16, 16);
          Graphics grfx   = Graphics.FromImage(bm);
          Font     font   = new Font("Arial", 10, FontStyle.Bold);
          SizeF    sizef  = grfx.MeasureString("A", font);
          float    fScale = Math.Min(bm.Width / sizef.Width, 
                                     bm.Height / sizef.Height);

          font = new Font(font.Name, fScale * font.SizeInPoints, font.Style);
          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = strfmt.LineAlignment = StringAlignment.Center;

          grfx.Clear(Color.White);
          grfx.DrawString("A", font, new SolidBrush(clr), 
                          bm.Width / 2, bm.Height / 2, strfmt);
          grfx.Dispose();

          return bm;
     }
}
class MenuItemColor: MenuItem
{
     Color clr;

     public Color Color 
     {
          get { return clr; }
          set { clr = value; }
     }
}