//----------------------------------------
// BlockFont.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BlockFont: FontMenuForm
{
     const int iReps = 50;    // Approximately 1/2 inch (exactly on printer)

     public new static void Main()
     {
          Application.Run(new BlockFont());
     }
     public BlockFont()
     {
          Text = "Block Font";
          Width *= 2;
          strText = "Block";
          font = new Font("Times New Roman", 108);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF sizef = grfx.MeasureString(strText, font);
          float x     = (cx - sizef.Width  - iReps) / 2;
          float y     = (cy - sizef.Height + iReps) / 2;

          grfx.Clear(Color.LightGray);

          for (int i = 0; i < iReps; i++)
               grfx.DrawString(strText, font, Brushes.Black, x + i, y - i);

          grfx.DrawString(strText, font, Brushes.White, x + iReps, 
                                                        y - iReps);
     }
}
