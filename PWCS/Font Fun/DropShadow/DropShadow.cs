//-----------------------------------------
// DropShadow.cs (R) 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DropShadow: FontMenuForm
{
     const int iOffset = 10;   // Approximately 1/10 inch (exactly on printer)

     public new static void Main()
     {
          Application.Run(new DropShadow());
     }
     public DropShadow()
     {
          Text = "Drop Shadow";
          Width *= 2;
          strText = "Shadow";
          font = new Font("Times New Roman", 108);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF sizef = grfx.MeasureString(strText, font);
          float x     = (cx - sizef.Width) / 2;
          float y     = (cy - sizef.Height) / 2;

          grfx.Clear(Color.White);
          grfx.DrawString(strText, font, Brushes.Gray, x, y);
          grfx.DrawString(strText, font, Brushes.Black, x - iOffset, 
                                                        y - iOffset);
     }
}
