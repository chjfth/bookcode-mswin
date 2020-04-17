//-------------------------------------------
// FamiliesList.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FamiliesList: PrintableForm
{
     const int iPointSize = 12;

     public new static void Main()
     {
          Application.Run(new FamiliesList());
     }
     public FamiliesList()
     {
          Text = "Font Families List";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush = new SolidBrush(clr);
          float        x = 0, y = 0, fMaxWidth = 0;
          FontFamily[] aff = GetFontFamilyArray(grfx);

          foreach (FontFamily ff in aff)
          {
               Font  font  = CreateSampleFont(ff, iPointSize);
               SizeF sizef = grfx.MeasureString(ff.Name, font);

               fMaxWidth = Math.Max(fMaxWidth, sizef.Width);
          }
          foreach (FontFamily ff in aff)
          {
               Font  font    = CreateSampleFont(ff, iPointSize);
               float fHeight = font.GetHeight(grfx);

               if (y > 0 && y + fHeight > cy)
               {
                    x += fMaxWidth;
                    y  = 0;
               }
               grfx.DrawString(ff.Name, font, brush, x, y);

               y += fHeight;
          }
     }
     protected virtual FontFamily[] GetFontFamilyArray(Graphics grfx)
     {
          return FontFamily.Families;
     }
     Font CreateSampleFont(FontFamily ff, float fPointSize)
     {
          if (ff.IsStyleAvailable(FontStyle.Regular))
               return new Font(ff, fPointSize);

          else if (ff.IsStyleAvailable(FontStyle.Bold))
               return new Font(ff, fPointSize, FontStyle.Bold);

          else if (ff.IsStyleAvailable(FontStyle.Italic))
               return new Font(ff, fPointSize, FontStyle.Italic);

          else
               return Font;
     }                                        
}
