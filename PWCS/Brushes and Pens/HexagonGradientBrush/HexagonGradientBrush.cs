//---------------------------------------------------
// HexagonGradientBrush.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HexagonGradientBrush: PrintableForm
{
     const float fSide = 50;       // Side (also radius) of hexagon

     public new static void Main()
     {
          Application.Run(new HexagonGradientBrush());
     }
     public HexagonGradientBrush()
     {
          Text = "Hexagon Gradient Brush";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
               // Calculate half the hexagon height.

          float fHalf = fSide * (float) Math.Sin(Math.PI / 3); 
          
               // Define a hexagon including some extra width

          PointF[] aptf = {new PointF( fSide,         0),           
                           new PointF( fSide * 1.5f,  0),
                           new PointF( fSide,         0),           
                           new PointF( fSide / 2,    -fHalf),
                           new PointF(-fSide / 2,    -fHalf),
                           new PointF(-fSide,         0),
                           new PointF(-fSide * 1.5f,  0), 
                           new PointF(-fSide,         0),
                           new PointF(-fSide / 2,     fHalf),     
                           new PointF( fSide / 2,     fHalf) };

               // Create the first brush.

          PathGradientBrush pgbrush1 =
                              new PathGradientBrush(aptf, WrapMode.Tile);

               // Offset the hexagon and define the second brush.

          for (int i = 0; i < aptf.Length; i++)
          {
               aptf[i].X += fSide * 1.5f;
               aptf[i].Y += fHalf;
          }
          PathGradientBrush pgbrush2 = 
                              new PathGradientBrush(aptf, WrapMode.Tile);

          grfx.FillRectangle(pgbrush1, 0, 0, cx, cy);
          grfx.FillRectangle(pgbrush2, 0, 0, cx, cy);
     }
}
