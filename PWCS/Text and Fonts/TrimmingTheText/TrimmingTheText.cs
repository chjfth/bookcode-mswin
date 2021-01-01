//----------------------------------------------
// TrimmingTheText.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TrimmingTheText:PrintableForm
{
     public new static void Main()
     {
          Application.Run(new TrimmingTheText());
     }
     public TrimmingTheText()
     {
          Text = "Trimming the Text";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          Brush        brush  = new SolidBrush(clr);
          float        cyText = Font.GetHeight(grfx);
          float        cyRect = cyText;
          RectangleF   rectf  = new RectangleF(0, 0, cx, cyRect);
          string       str    = "Those who profess to favor freedom and " +
                                "yet depreciate agitation. . .want " +
			                    "crops without plowing up the ground, " +
                                "they want rain without thunder and " +
                                "lightning. They want the ocean without " +
                                "the awful roar of its many waters. " +
                                "\x2014 Frederick Douglass";
          StringFormat strfmt = new StringFormat();

          strfmt.Trimming = StringTrimming.Character;
          grfx.DrawString("Character: " + str, Font, brush, rectf, strfmt);

          rectf.Offset(0, cyRect + cyText);

          strfmt.Trimming = StringTrimming.Word;
          grfx.DrawString("Word: " + str, Font, brush, rectf, strfmt);

          rectf.Offset(0, cyRect + cyText);

          strfmt.Trimming = StringTrimming.EllipsisCharacter;
          grfx.DrawString("EllipsisCharacter: " + str, 
                          Font, brush, rectf, strfmt);

          rectf.Offset(0, cyRect + cyText);

          strfmt.Trimming = StringTrimming.EllipsisWord;
          grfx.DrawString("EllipsisWord: " + str, 
                          Font, brush, rectf, strfmt);

          rectf.Offset(0, cyRect + cyText);

          strfmt.Trimming = StringTrimming.EllipsisPath;
          grfx.DrawString("EllipsisPath: " + 
                          Environment.GetFolderPath
                               (Environment.SpecialFolder.Personal),
                          Font, brush, rectf, strfmt);

          rectf.Offset(0, cyRect + cyText);

          strfmt.Trimming = StringTrimming.None;
          grfx.DrawString("None: " + str, Font, brush, rectf, strfmt);
     }
}
