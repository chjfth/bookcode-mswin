//-------------------------------------------
// AllAboutFont.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class AllAboutFont: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new AllAboutFont());
     }
     public AllAboutFont()
     {
          Text = "All About Font";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawString(
                    "Name: "               + Font.Name            + "\n" +
                    "FontFamily: "         + Font.FontFamily      + "\n" +
                    "FontStyle: "          + Font.Style           + "\n" +
                    "Bold: "               + Font.Bold            + "\n" +
                    "Italic: "             + Font.Italic          + "\n" +
                    "Underline: "          + Font.Underline       + "\n" +
                    "Strikeout: "          + Font.Strikeout       + "\n" +
                    "Size: "               + Font.Size            + "\n" +
                    "GraphicsUnit: "       + Font.Unit            + "\n" +
                    "SizeInPoints: "       + Font.SizeInPoints    + "\n" +
                    "Height: "             + Font.Height          + "\n" +
                    "GdiCharSet: "         + Font.GdiCharSet      + "\n" +
                    "GdiVerticalFont : "   + Font.GdiVerticalFont + "\n" +
                    "GetHeight(): "        + Font.GetHeight()     + "\n" +
                    "GetHeight(grfx): "    + Font.GetHeight(grfx) + "\n" +
                    "GetHeight(100 DPI): " + Font.GetHeight(100),
                    Font, new SolidBrush(clr), Point.Empty);
     }
}
