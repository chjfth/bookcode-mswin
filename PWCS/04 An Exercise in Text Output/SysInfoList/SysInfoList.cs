//------------------------------------------
// SysInfoList.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoList: Form
{
     readonly float cxCol;
     readonly int   cySpace;

     public static void Main()
     {
          Application.Run(new SysInfoList());
     }
     public SysInfoList()
     {
          Text = "System Information: List";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          Graphics grfx  = CreateGraphics();
          SizeF    sizef = grfx.MeasureString(" ", Font);
          cxCol = sizef.Width + SysInfoStrings.MaxLabelWidth(grfx, Font);
          grfx.Dispose();
          cySpace = Font.Height;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx       = pea.Graphics;
          Brush    brush      = new SolidBrush(ForeColor);
          int      iCount     = SysInfoStrings.Count;
          string[] astrLabels = SysInfoStrings.Labels;
          string[] astrValues = SysInfoStrings.Values;

          for (int i = 0; i < iCount; i++)
          {
               grfx.DrawString(astrLabels[i], Font, brush, 
                               0, i * cySpace);
               grfx.DrawString(astrValues[i], Font, brush, 
                               cxCol, i * cySpace); 
          }
     }
}
