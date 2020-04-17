//------------------------------------------
// JeuDeTaquin.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class JeuDeTaquin: Form
{
     const int          nRows = 4;
     const int          nCols = 4;
     Size               sizeTile;
     JeuDeTaquinTile[,] atile = new JeuDeTaquinTile[nRows, nCols];
     Random             rand;
     Point              ptBlank;
     int                iTimerCountdown;

     public static void Main()
     {
          Application.Run(new JeuDeTaquin());
     }
     public JeuDeTaquin()
     {
          Text = "Jeu de Taquin";
          FormBorderStyle = FormBorderStyle.Fixed3D;
     }
     protected override void OnLoad(EventArgs ea)
     {
               // Calculate the size of the tiles and the form.

          Graphics grfx = CreateGraphics();

          sizeTile   = new Size((int)(2 * grfx.DpiX / 3),
                                (int)(2 * grfx.DpiY / 3));
          ClientSize = new Size(nCols * sizeTile.Width,
                                nRows * sizeTile.Height);
          grfx.Dispose();

               // Create the tiles.

          for (int iRow = 0; iRow < nRows; iRow++)
          for (int iCol = 0; iCol < nCols; iCol++)
          {
               int iNum = iRow  * nCols + iCol + 1;

               if (iNum == nRows * nCols)
                    continue;

               JeuDeTaquinTile tile = new JeuDeTaquinTile(iNum);
               tile.Parent = this;
               tile.Location = new Point(iCol * sizeTile.Width,
                                         iRow * sizeTile.Height);
               tile.Size = sizeTile;

               atile[iRow, iCol] = tile;
          }
          ptBlank = new Point(nCols - 1, nRows - 1);

          Randomize();
     }
     protected void Randomize()
     {
          rand = new Random();
          iTimerCountdown = 64 * nRows * nCols;

          Timer timer    = new Timer();
          timer.Tick    += new EventHandler(TimerOnTick);
          timer.Interval = 1;
          timer.Enabled  = true;
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          int x = ptBlank.X;
          int y = ptBlank.Y;

          switch(rand.Next(4))
          {
          case 0:  x++;  break;
          case 1:  x--;  break;
          case 2:  y++;  break;
          case 3:  y--;  break;
          }
          if (x >= 0 && x < nCols && y >= 0 && y < nRows)
               MoveTile(x, y);

          if (--iTimerCountdown == 0)
          {
              ((Timer)obj).Stop();
              ((Timer)obj).Tick -= new EventHandler(TimerOnTick);
          }
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          if (kea.KeyCode == Keys.Left && ptBlank.X < nCols - 1)
               MoveTile(ptBlank.X + 1, ptBlank.Y);

          else if (kea.KeyCode == Keys.Right && ptBlank.X > 0)
               MoveTile(ptBlank.X - 1, ptBlank.Y);

          else if (kea.KeyCode == Keys.Up && ptBlank.Y < nRows - 1)
               MoveTile(ptBlank.X, ptBlank.Y + 1);

          else if (kea.KeyCode == Keys.Down && ptBlank.Y > 0)
               MoveTile(ptBlank.X, ptBlank.Y - 1);

          kea.Handled = true;
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          int x = mea.X / sizeTile.Width;
          int y = mea.Y / sizeTile.Height;

          if (x == ptBlank.X)
          {
               if (y < ptBlank.Y)
                    for (int y2 = ptBlank.Y - 1; y2 >= y; y2--)
                         MoveTile(x, y2);

               else if (y > ptBlank.Y)
                    for (int y2 = ptBlank.Y + 1; y2 <= y; y2++)
                         MoveTile(x, y2);
          }
          else if (y == ptBlank.Y)
          {
               if (x < ptBlank.X)
                    for (int x2 = ptBlank.X - 1; x2 >= x; x2--)
                         MoveTile(x2, y);

               else if (x > ptBlank.X)
                    for (int x2 = ptBlank.X + 1; x2 <= x; x2++)
                         MoveTile(x2, y);
          }
     }
     void MoveTile(int x, int y)
     {
          atile[y, x].Location = new Point(ptBlank.X * sizeTile.Width,
                                           ptBlank.Y * sizeTile.Height);

          atile[ptBlank.Y, ptBlank.X] = atile[y, x];
          atile[y, x] = null;
          ptBlank = new Point(x, y);
     }
}

