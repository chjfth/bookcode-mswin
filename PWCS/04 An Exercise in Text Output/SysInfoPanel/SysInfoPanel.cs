//-------------------------------------------
// SysInfoPanel.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoPanel: Form
{
     readonly float cxCol;
     readonly int   cySpace;

     public static void Main()
     {
          Application.Run(new SysInfoPanel());
     }
     public SysInfoPanel()
     {
          Text = "System Information: Panel";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          AutoScroll = true;

          Graphics grfx  = CreateGraphics();
          SizeF    sizef = grfx.MeasureString(" ", Font);
          cxCol   = sizef.Width + SysInfoStrings.MaxLabelWidth(grfx, Font);
          cySpace = Font.Height;

               // Create a panel.

          Panel panel    = new Panel();
          panel.Parent   = this;
          panel.Paint   += new PaintEventHandler(PanelOnPaint);
          panel.Location = Point.Empty;
          panel.Size     = new Size(
              (int) Math.Ceiling(cxCol + 
                                 SysInfoStrings.MaxValueWidth(grfx, Font)),
              (int) Math.Ceiling(cySpace * SysInfoStrings.Count));
          grfx.Dispose();
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
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
