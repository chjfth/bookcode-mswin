//---------------------------------------
// PieChart.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PieChart: PrintableForm
{
     int[] aiValues = { 50, 100, 25, 150, 100, 75 };

     public new static void Main()
     {
          Application.Run(new PieChart());
     }
     public PieChart()
     {
          Text = "Pie Chart";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Rectangle rect   = new Rectangle(50, 50, 200, 200);
          Pen       pen    = new Pen(clr);
          int       iTotal = 0;
          float     fAngle = 0, fSweep;

          foreach(int iValue in aiValues)
               iTotal += iValue;

          foreach(int iValue in aiValues)
          {
               fSweep = 360f * iValue / iTotal;
               DrawPieSlice(grfx, pen, rect, fAngle, fSweep);
               fAngle += fSweep;
          }
     }
     protected virtual void DrawPieSlice(Graphics grfx, Pen pen, 
                                         Rectangle rect,
                                         float fAngle, float fSweep)
     {
          grfx.DrawPie(pen, rect, fAngle, fSweep); 
     }
}
