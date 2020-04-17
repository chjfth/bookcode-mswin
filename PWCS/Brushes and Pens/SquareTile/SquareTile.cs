//-----------------------------------------
// SquareTile © 20.cs01 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class SquareTile: PrintableForm
{
     const int iSide = 50;         // Side of square

     public new static void Main()
     {
          Application.Run(new SquareTile());
     }
     public SquareTile()
     {
          Text = "Square Tile";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Point[] apt = {new Point(0,     0),     new Point(iSide, 0), 
                         new Point(iSide, iSide), new Point(0,     iSide)};

          PathGradientBrush pgbrush =
                         new PathGradientBrush(apt, WrapMode.TileFlipXY);

          pgbrush.SurroundColors = new Color[] { Color.Red,  Color.Lime,
                                                 Color.Blue, Color.White};

          grfx.FillRectangle(pgbrush, 0, 0, cx, cy);
     }
}
