//--------------------------------------------
// SysInfoScroll.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoScroll: Form
{
     readonly float cxCol;
     readonly int   cySpace;

     public static void Main()
     {
          Application.Run(new SysInfoScroll());
     }
     public SysInfoScroll()
     {
          Text = "System Information: Scroll";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          Graphics grfx  = CreateGraphics();
          SizeF    sizef = grfx.MeasureString(" ", Font);
          cxCol   = sizef.Width + SysInfoStrings.MaxLabelWidth(grfx, Font);
          cySpace = Font.Height;

               // Set auto-scroll properties.

          AutoScroll = true;
          AutoScrollMinSize = new Size(
              (int) Math.Ceiling(cxCol + 
                                 SysInfoStrings.MaxValueWidth(grfx, Font)),
              (int) Math.Ceiling(cySpace * SysInfoStrings.Count));

          grfx.Dispose();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx       = pea.Graphics;
          Brush    brush      = new SolidBrush(ForeColor);
          int      iCount     = SysInfoStrings.Count;
          string[] astrLabels = SysInfoStrings.Labels;
          string[] astrValues = SysInfoStrings.Values;
          Point    pt         = AutoScrollPosition;

          for (int i = 0; i < iCount; i++)
          {
               grfx.DrawString(astrLabels[i], Font, brush, 
                               pt.X, pt.Y + i * cySpace);

               grfx.DrawString(astrValues[i], Font, brush, 
                               pt.X + cxCol, pt.Y + i * cySpace); 
          }
     }
}
