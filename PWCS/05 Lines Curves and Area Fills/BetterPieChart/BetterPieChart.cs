//---------------------------------------------
// BetterPieChart.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterPieChart: PieChart
{
     public new static void Main()
     {
          Application.Run(new BetterPieChart());
     }
     public BetterPieChart()
     {
          Text = "Better " + Text;
     }
     protected override void DrawPieSlice(Graphics grfx, Pen pen, 
                                          Rectangle rect,
                                          float fAngle, float fSweep)
     {
          float fSlice = (float)(2 * Math.PI * (fAngle + fSweep / 2) / 360);

          rect.Offset((int)(rect.Width  / 10 * Math.Cos(fSlice)),
                      (int)(rect.Height / 10 * Math.Sin(fSlice)));

          base.DrawPieSlice(grfx, pen, rect, fAngle, fSweep);
     }
}
