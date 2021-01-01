//------------------------------------------------------
// OverlappingHatchBrushes.cs (R) 2001 by Charles Petzold
//------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class OverlappingHatchBrushes: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new OverlappingHatchBrushes());
     }
     public OverlappingHatchBrushes()
     {
          Text = "Overlapping Hatch Brushes";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          HatchBrush hbrush = new HatchBrush(HatchStyle.HorizontalBrick, 
                                             Color.White);
          for (int i = 0; i < 10; i++)
               grfx.FillRectangle(hbrush, i * cx / 10, i * cy / 10, 
                                          cx / 8, cy / 8);
     }
}
