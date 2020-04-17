//-------------------------------------------
// TriangleTile.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TriangleTile: PrintableForm
{
     const int iSide = 50;         // Side of square for triangle
     MenuItem  miChecked;

     public new static void Main()
     {
          Application.Run(new TriangleTile());
     }
     public TriangleTile()
     {
          Text = "Triangle Tile";

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
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Point[] apt = { new Point(0,     0), 
                          new Point(iSide, 0), 
                          new Point(0,     iSide)};

          PathGradientBrush pgbrush = 
                    new PathGradientBrush(apt, (WrapMode) miChecked.Index);

          grfx.FillRectangle(pgbrush, 0, 0, cx, cy);
     }
}
