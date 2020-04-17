//---------------------------------------------
// UnderlinedText.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;

class UnderlinedText: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new UnderlinedText());
     }
     public UnderlinedText()
     {
          Text = "Underlined Text Using HotkeyPrefix";
          Font = new Font("Times New Roman", 14);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          string       str    = "This is some &u&n&d&e&r&l&i&n&e&d text!";
          StringFormat strfmt = new StringFormat();

          strfmt.HotkeyPrefix = HotkeyPrefix.Show;

          grfx.DrawString(str, Font, new SolidBrush(clr), 0, 0, strfmt);
     }
}

