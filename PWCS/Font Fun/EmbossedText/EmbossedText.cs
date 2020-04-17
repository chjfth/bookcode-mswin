//-------------------------------------------
// EmbossedText.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class EmbossedText: FontMenuForm
{
     int iOffset = 2;

     public new static void Main()
     {
          Application.Run(new EmbossedText());
     }
     public EmbossedText()
     {
          Text = "Embossed Text";
          Width *= 2;
          Menu.MenuItems.Add("&Toggle!", 
                                   new EventHandler(MenuToggleOnClick));
          strText = "Emboss";
          font = new Font("Times New Roman", 108);
     }
     void MenuToggleOnClick(object obj, EventArgs ea)
     {
          iOffset = -iOffset;
          Text = (iOffset > 0) ? "Embossed Text" : "Engraved Text";
          strText = (iOffset > 0) ? "Emboss" : "Engrave";
          Invalidate();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF sizef = grfx.MeasureString(strText, font);
          float x     = (cx - sizef.Width) / 2;
          float y     = (cy - sizef.Height) / 2;

          grfx.Clear(Color.White);
          grfx.DrawString(strText, font, Brushes.Gray, x, y);
          grfx.DrawString(strText, font, Brushes.White, x - iOffset, 
                                                        y - iOffset);
     }
}
