//------------------------------------------
// FourByFours.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FourByFours: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new FourByFours());
     }
     public FourByFours()
     {
          Text = "Four by Fours";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy) 
     {
          Pen   pen   = new Pen(clr);
          Brush brush = new SolidBrush(clr);
          
          grfx.DrawRectangle(pen, new Rectangle(2, 2, 4, 4));
          grfx.DrawRectangles(pen, new Rectangle[] 
                                             {new Rectangle(8, 2, 4, 4)});
          grfx.DrawEllipse(pen, new Rectangle(14, 2, 4, 4));

          grfx.FillRectangle(brush, new Rectangle(2, 8, 4, 4));
          grfx.FillRectangles(brush, new Rectangle[] 
                                             {new Rectangle(8, 8, 4, 4)});
          grfx.FillEllipse(brush, new Rectangle(14, 8, 4, 4));
     }
}
