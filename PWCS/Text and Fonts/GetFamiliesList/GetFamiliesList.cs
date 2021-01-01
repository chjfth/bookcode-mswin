//----------------------------------------------
// GetFamiliesList.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class GetFamiliesList: FamiliesList
{
     public new static void Main()
     {
          Application.Run(new GetFamiliesList());
     }
     public GetFamiliesList()
     {
          Text = "Font GetFamilies List";
     }
     protected override FontFamily[] GetFontFamilyArray(Graphics grfx)
     {
          return FontFamily.GetFamilies(grfx);
     }
}
