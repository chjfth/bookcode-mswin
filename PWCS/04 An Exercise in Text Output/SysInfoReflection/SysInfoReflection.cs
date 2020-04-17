//-----------------------------------------------
// SysInfoReflection.cs © 2001 by Charles Petzold
//-----------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoReflection: Form
{
     protected int      iCount;
     protected string[] astrLabels;
     protected string[] astrValues;
     protected float    cxCol;
     protected int      cySpace;

     public static void Main()
     {
          Application.Run(new SysInfoReflection());
     }
     public SysInfoReflection()
     {
          Text = "System Information: Reflection";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          AutoScroll = true;

          SystemEvents.UserPreferenceChanged += 
               new UserPreferenceChangedEventHandler(UserPreferenceChanged);

          SystemEvents.DisplaySettingsChanged +=
               new EventHandler(DisplaySettingsChanged);

          UpdateAllInfo();
     }
     void UserPreferenceChanged(object obj, UserPreferenceChangedEventArgs ea)
     {
          UpdateAllInfo();
          Invalidate();
     }
     void DisplaySettingsChanged(object obj, EventArgs ea)
     {
          UpdateAllInfo();
          Invalidate();
     }
     void UpdateAllInfo()
     {
          iCount     = SysInfoReflectionStrings.Count;
          astrLabels = SysInfoReflectionStrings.Labels;
          astrValues = SysInfoReflectionStrings.Values;

          Graphics grfx  = CreateGraphics();
          SizeF    sizef = grfx.MeasureString(" ", Font);
          cxCol  = sizef.Width + 
                         SysInfoReflectionStrings.MaxLabelWidth(grfx, Font);
          cySpace = Font.Height;

          AutoScrollMinSize = new Size(
              (int) Math.Ceiling(cxCol + 
                         SysInfoReflectionStrings.MaxValueWidth(grfx, Font)),
              (int) Math.Ceiling(cySpace * iCount));

          grfx.Dispose();
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
