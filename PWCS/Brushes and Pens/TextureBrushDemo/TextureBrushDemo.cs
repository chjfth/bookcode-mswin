//-----------------------------------------------
// TextureBrushDemo.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TextureBrushDemo: PrintableForm
{
     MenuItem     miChecked;
     TextureBrush tbrush;

     public new static void Main()
     {
          Application.Run(new TextureBrushDemo());
     } 
     public TextureBrushDemo()
     {
          Text = "Texture Brush Demo";

          Image image = Image.FromFile(
               "..\\..\\..\\..\\Images and Bitmaps\\Apollo11FullColor.jpg");

          tbrush = new TextureBrush(image, new Rectangle(95, 0, 50, 55));

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Wrap-Mode");

          foreach (WrapMode wm in Enum.GetValues(typeof(WrapMode)))
          {
               MenuItem mi = new MenuItem();
               mi.Text     = wm.ToString(); 
               mi.Click   += new EventHandler(MenuWrapModeOnClick);
               Menu.MenuItems[0].MenuItems.Add(mi);
          }
          miChecked = Menu.MenuItems[0].MenuItems[0];
          miChecked.Checked = true;
     }
     void MenuWrapModeOnClick(object obj, EventArgs ea)
     {
          miChecked.Checked = false;
          miChecked = (MenuItem) obj;
          miChecked.Checked = true;

          tbrush.WrapMode = (WrapMode)miChecked.Index;
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.FillEllipse(tbrush, 0, 0, 2 * cx / 3, 2 * cy / 3);
          grfx.FillEllipse(tbrush, cx / 3, cy / 3, 2 * cx / 3, 2 * cy / 3);
     }
}
