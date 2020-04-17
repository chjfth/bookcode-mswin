//----------------------------------------------
// FillModesOddity.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class FillModesOddity: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new FillModesOddity());
     }
     public FillModesOddity()
     {
          Text = "Alternate and Winding Fill Modes (An Oddity)";
          ClientSize = new Size(2 * ClientSize.Height, ClientSize.Height);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush    brush = new SolidBrush(clr);
          PointF[] aptf = { new PointF(0.1f, 0.7f), new PointF(0.5f, 0.7f),
                            new PointF(0.5f, 0.1f), new PointF(0.9f, 0.1f),
                            new PointF(0.9f, 0.5f), new PointF(0.3f, 0.5f),
                            new PointF(0.3f, 0.9f), new PointF(0.7f, 0.9f),
                            new PointF(0.7f, 0.3f), new PointF(0.1f, 0.3f)};
          
          for (int i = 0; i < aptf.Length; i++)
          {
               aptf[i].X *= cx / 2;
               aptf[i].Y *= cy;
          }
          grfx.FillPolygon(brush, aptf, FillMode.Alternate);

          for (int i = 0; i < aptf.Length; i++)
               aptf[i].X += cx / 2;

          grfx.FillPolygon(brush, aptf, FillMode.Winding);
     }
}
