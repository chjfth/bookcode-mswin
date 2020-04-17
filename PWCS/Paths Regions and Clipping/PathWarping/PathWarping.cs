//------------------------------------------
// PathWarping.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class PathWarping: Form
{
     MenuItem     miWarpMode;
     GraphicsPath path;
     PointF[]     aptfDest = new PointF[4];

     public static void Main()
     {
          Application.Run(new PathWarping());
     }
     public PathWarping()
     {
          Text = "Path Warping";

               // Create menu.

          Menu = new MainMenu();

          Menu.MenuItems.Add("&Warp Mode");
          EventHandler ehClick = new EventHandler(MenuWarpModeOnClick);

          miWarpMode = new MenuItem("&" + (WarpMode)0, ehClick);
          miWarpMode.RadioCheck = true;
          miWarpMode.Checked = true;
          Menu.MenuItems[0].MenuItems.Add(miWarpMode);

          MenuItem mi = new MenuItem("&" + (WarpMode)1, ehClick);
          mi.RadioCheck = true;
          Menu.MenuItems[0].MenuItems.Add(mi);

               // Create path.

          path = new GraphicsPath();

          for (int i = 0; i <= 8; i++)
          {
               path.StartFigure();
               path.AddLine(0, 100 * i, 800, 100 * i);
               path.StartFigure();
               path.AddLine(100 * i, 0, 100 * i, 800);
          }
               // Initialize Point array.

          aptfDest[0] = new Point( 50,  50);
          aptfDest[1] = new Point(200,  50);
          aptfDest[2] = new Point( 50, 200);
          aptfDest[3] = new Point(200, 200);
     }
     void MenuWarpModeOnClick(object obj, EventArgs ea)
     {
          miWarpMode.Checked = false;
          miWarpMode = (MenuItem) obj;
          miWarpMode.Checked = true;

          Invalidate();
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          Point pt;

          if (mea.Button == MouseButtons.Left)
          {
               if (ModifierKeys == Keys.None)
                    pt = Point.Round(aptfDest[0]);
               else if (ModifierKeys == Keys.Shift)
                    pt = Point.Round(aptfDest[2]);
               else
                    return;
          }
          else if (mea.Button == MouseButtons.Right)
          {
               if (ModifierKeys == Keys.None)
                    pt = Point.Round(aptfDest[1]);
               else if (ModifierKeys == Keys.Shift)
                    pt = Point.Round(aptfDest[3]);
               else
                    return;
          }
          else
               return;

          Cursor.Position = PointToScreen(pt);
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          Point pt = new Point(mea.X, mea.Y);

          if (mea.Button == MouseButtons.Left)
          {
               if (ModifierKeys == Keys.None)
                    aptfDest[0] = pt;
               else if (ModifierKeys == Keys.Shift)
                    aptfDest[2] = pt;
               else
                    return;
          }
          else if (mea.Button == MouseButtons.Right)
          {
               if (ModifierKeys == Keys.None)
                    aptfDest[1] = pt;
               else if (ModifierKeys == Keys.Shift)
                    aptfDest[3] = pt;
               else
                    return;
          }
          else
               return;

          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics     grfx       = pea.Graphics;
          GraphicsPath pathWarped = (GraphicsPath) path.Clone();
          WarpMode     wm         = (WarpMode) miWarpMode.Index;

          pathWarped.Warp(aptfDest, path.GetBounds(), new Matrix(), wm);
          grfx.DrawPath(new Pen(ForeColor), pathWarped);
     }
}