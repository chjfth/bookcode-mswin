//-------------------------------------
// Bricks.cs (R) 2001 by Charles Petzold
//-------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class Bricks: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new Bricks());
     }
     public Bricks()
     {
          Text = "Bricks";

          strText = "Bricks";
          font = new Font("Times New Roman", 144);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF sizef  = grfx.MeasureString(strText, font);
          Brush hbrush = new HatchBrush(HatchStyle.HorizontalBrick,
                                        Color.White, Color.Black);

          grfx.DrawString(strText, font, hbrush, (cx - sizef.Width) / 2,
                                                 (cy - sizef.Height) / 2);
     }
}