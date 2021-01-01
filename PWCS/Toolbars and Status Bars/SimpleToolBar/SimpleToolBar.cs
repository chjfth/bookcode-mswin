//--------------------------------------------
// SimpleToolBar.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleToolBar: Form
{
     public static void Main()
     {
          Application.Run(new SimpleToolBar());
     }
     public SimpleToolBar()
     {
          Text = "Simple Toolbar";

               // Create a simple menu (just for show).

          Menu = new MainMenu();
          Menu.MenuItems.Add("File");
          Menu.MenuItems.Add("Edit");
          Menu.MenuItems.Add("View");
          Menu.MenuItems.Add("Help");

               // Create ImageList.

          Bitmap bm = new Bitmap(GetType(), 
                                 "SimpleToolBar.StandardButtons.bmp");

          ImageList imglst = new ImageList();
          imglst.Images.AddStrip(bm);
          imglst.TransparentColor = Color.Cyan;

               // Create ToolBar.

          ToolBar tbar = new ToolBar();
          tbar.Parent = this;
          tbar.ImageList = imglst;
          tbar.ShowToolTips = true;

               // Create ToolBarButtons.

          string[] astr = {"New", "Open", "Save", "Print", 
                           "Cut", "Copy", "Paste" };

          for (int i = 0; i < 7; i++)
          {
               ToolBarButton tbarbtn = new ToolBarButton();
               tbarbtn.ImageIndex = i;
               tbarbtn.ToolTipText = astr[i];

               tbar.Buttons.Add(tbarbtn);
          }
     }
}
