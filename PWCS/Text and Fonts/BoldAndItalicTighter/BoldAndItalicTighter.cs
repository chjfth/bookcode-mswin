//---------------------------------------------------
// BoldAndItalicTighter.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;

class BoldAndItalicTighter: PrintableForm
{
	public new static void Main()
	{
		Application.Run(new BoldAndItalicTighter());
	}
	public BoldAndItalicTighter()
	{
		Text = "Bold and Italic (Tighter)";
		Font = new Font("Times New Roman", 24);
	}
	protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
	{
		const string str1        = "This is some ";
		const string str2        = "bold";
		const string str3        = " text, and this is some ";
		const string str4        = "italic";
		const string str5        = " text.";
		Brush        brush       = new SolidBrush(clr);
		Font         fontRegular = Font;
		Font         fontBold    = new Font(fontRegular, FontStyle.Bold);
		Font         fontItalic  = new Font(fontRegular, FontStyle.Italic);
		PointF       ptf         = new PointF(0, 0);
		StringFormat strfmt      = StringFormat.GenericTypographic;
		strfmt.FormatFlags      |= StringFormatFlags.MeasureTrailingSpaces;

		grfx.DrawString(str1, fontRegular, brush, ptf, strfmt);
		ptf.X += grfx.MeasureString(str1, fontRegular, ptf, strfmt).Width;

		grfx.DrawString(str2, fontBold, brush, ptf, strfmt);
		ptf.X += grfx.MeasureString(str2, fontBold, ptf, strfmt).Width;

		grfx.DrawString(str3, fontRegular, brush, ptf, strfmt);
		ptf.X += grfx.MeasureString(str3, fontRegular, ptf, strfmt).Width;

		grfx.DrawString(str4, fontItalic, brush, ptf, strfmt);
		ptf.X += grfx.MeasureString(str4, fontItalic, ptf, strfmt).Width;

		grfx.DrawString(str5, fontRegular, brush, ptf, strfmt);
	}
}
