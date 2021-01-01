//---------------------------------------
// WarpText.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class WarpText: FontMenuForm
{
     int iWarpMode = 0;

     public new static void Main()
     {
          Application.Run(new WarpText());
     }
     public WarpText()
     {
          Text = "Warp Text - " + (WarpMode) iWarpMode;
          Menu.MenuItems.Add("&Toggle!", 
                                   new EventHandler(MenuToggleOnClick));
          strText = "WARP";
          font = new Font("Arial Black", 24);
     }
     void MenuToggleOnClick(object obj, EventArgs ea)
     {
          iWarpMode ^= 1;
          Text = "Warp Text - " + (WarpMode) iWarpMode;
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          GraphicsPath path = new GraphicsPath();

               // Add text to the path.

          path.AddString(strText, font.FontFamily, (int) font.Style,
                         100, new PointF(0, 0), new StringFormat());

               // Warp the path.

          RectangleF rectfBounds = path.GetBounds();
          PointF[]   aptfDest    = { new PointF(cx / 3, 0),
                                     new PointF(2 * cx / 3, 0),
                                     new PointF(0, cy),
                                     new PointF(cx, cy) };

          path.Warp(aptfDest, rectfBounds, new Matrix(), 
                    (WarpMode) iWarpMode);

               // Fill the path.

          grfx.FillPath(new SolidBrush(clr), path);
     }
}