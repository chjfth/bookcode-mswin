//--------------------------------------------------------
// HatchBrushRenderingOrigin.cs (R) 2001 by Charles Petzold
//--------------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class HatchBrushRenderingOrigin: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new HatchBrushRenderingOrigin());
     }
     public HatchBrushRenderingOrigin()
     {
          Text = "Hatch Brush Rendering Origin";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          HatchBrush hbrush = new HatchBrush(HatchStyle.HorizontalBrick, 
                                             Color.White);
          for (int i = 0; i < 10; i++)
          {
               grfx.RenderingOrigin = new Point(i * cx / 10, i * cy / 10);

               grfx.FillRectangle(hbrush, i * cx / 10, i * cy / 10, 
                                          cx / 8, cy / 8);
          }
     }
}
