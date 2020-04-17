//-------------------------------------------
// FontMenuForm.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FontMenuForm: PrintableForm
{
     protected string strText = "Sample Text";
     protected Font font = new Font("Times New Roman", 24, FontStyle.Italic);

     public new static void Main()
     {
          Application.Run(new FontMenuForm());
     }
     public FontMenuForm()
     {
          Text = "Font Menu Form";
          Menu = new MainMenu();
          Menu.MenuItems.Add("&Font!", new EventHandler(MenuFontOnClick));
     }
     void MenuFontOnClick(object obj, EventArgs ea)
     {
          FontDialog dlg = new FontDialog();
          dlg.Font = font;

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               font = dlg.Font;
               Invalidate();
          }
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF sizef = grfx.MeasureString(strText, font);
          Brush brush = new SolidBrush(clr);

          grfx.DrawString(strText, font, brush, (cx - sizef.Width) / 2,
                                                (cy - sizef.Height) / 2);
     }
     public float GetAscent(Graphics grfx, Font font)
     {
          return font.GetHeight(grfx) * 
                    font.FontFamily.GetCellAscent(font.Style) /
                         font.FontFamily.GetLineSpacing(font.Style);
     }
     public float GetDescent(Graphics grfx, Font font)
     {
          return font.GetHeight(grfx) * 
                    font.FontFamily.GetCellDescent(font.Style) /
                         font.FontFamily.GetLineSpacing(font.Style);
     }
     public float PointsToPageUnits(Graphics grfx, Font font)
     {
          float fFontSize;

          if (grfx.PageUnit == GraphicsUnit.Display)
               fFontSize = 100 * font.SizeInPoints / 72;
          else
               fFontSize = grfx.DpiX * font.SizeInPoints / 72;

          return fFontSize;
     }
}