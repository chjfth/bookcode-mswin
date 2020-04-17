//-----------------------------------------------
// SysInfoEfficient.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoEfficient: SysInfoUpdate
{
     public new static void Main()
     {
          Application.Run(new SysInfoEfficient());
     }
     public SysInfoEfficient()
     {
          Text = "System Information: Efficient";
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx  = pea.Graphics;
          Brush    brush = new SolidBrush(ForeColor);
          Point    pt    = AutoScrollPosition;

          int iFirst = (int)((pea.ClipRectangle.Top    - pt.Y) / cySpace);
          int iLast  = (int)((pea.ClipRectangle.Bottom - pt.Y) / cySpace);

          iLast = Math.Min(iCount - 1, iLast);

          for (int i = iFirst; i <= iLast; i++)
          {
               grfx.DrawString(astrLabels[i], Font, brush, 
                               pt.X, pt.Y + i * cySpace);

               grfx.DrawString(astrValues[i], Font, brush, 
                               pt.X + cxCol, pt.Y + i * cySpace); 
          }
     }
}
