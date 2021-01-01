//---------------------------------------
// DualWink.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DualWink: Wink
{
     Image[] aimageRev = new Image[4];

     public new static void Main()
     {
          Application.Run(new DualWink());
     }
     public DualWink()
     {
          Text = "Dual " + Text;

          for(int i = 0; i < 4; i++)
          {
               aimageRev[i] = (Image) aimage[i].Clone();
               aimageRev[i].RotateFlip(RotateFlipType.RotateNoneFlipX);
          }
     }
     protected override void TimerOnTick(object obj, EventArgs ea)
     {
          Graphics grfx = CreateGraphics();

          grfx.DrawImage(aimage[iImage], 
                    ClientSize.Width / 2,
                    (ClientSize.Height - aimage[iImage].Height) / 2,
					aimage[iImage].Width, aimage[iImage].Height);

          grfx.DrawImage(aimageRev[3 - iImage],
                    ClientSize.Width / 2 - aimageRev[3 - iImage].Width,
                    (ClientSize.Height - aimageRev[3 - iImage].Height) / 2,
					aimageRev[3 - iImage].Width,
					aimageRev[3 - iImage].Height);

          grfx.Dispose();

          iImage += iIncr;

          if (iImage == 3)
               iIncr = -1;
          
          else if (iImage == 0)
               iIncr = 1;
     }
}
