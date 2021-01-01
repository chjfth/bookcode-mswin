//----------------------------------------------
// TwoTriangleTile.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class TwoTriangleTile: PrintableForm
{
     const int iSide = 50;         // Side of square for triangle

     public new static void Main()
     {
          Application.Run(new TwoTriangleTile());
     }
     public TwoTriangleTile()
     {
          Text = "Two-Triangle Tile";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
               // Define the triangle and create the first brush.

          Point[] apt = 
               {new Point(0, 0), new Point(iSide, 0), new Point(0, iSide)};

          PathGradientBrush pgbrush1 = 
                         new PathGradientBrush(apt, WrapMode.TileFlipXY);

               // Define another triangle and create the second brush.

          apt = new Point[] {new Point(iSide, 0), new Point(iSide, iSide), 
                             new Point(0, iSide)};

          PathGradientBrush pgbrush2 = 
                         new PathGradientBrush(apt, WrapMode.TileFlipXY);

          grfx.FillRectangle(pgbrush1, 0, 0, cx, cy);
          grfx.FillRectangle(pgbrush2, 0, 0, cx, cy);
     }
}