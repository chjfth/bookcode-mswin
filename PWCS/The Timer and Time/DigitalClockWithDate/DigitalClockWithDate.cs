//---------------------------------------------------
// DigitalClockWithDate.cs © 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DigitalClockWithDate: DigitalClock
{
     public new static void Main()
     {
          Application.Run(new DigitalClockWithDate());
     }
     public DigitalClockWithDate()
     {
          Text += " with Date";
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx    = pea.Graphics;
          DateTime dt      = DateTime.Now;
          string   strTime = dt.ToString("d") + "\n" + dt.ToString("T");
          SizeF    sizef   = grfx.MeasureString(strTime, Font);
          float    fScale  = Math.Min(ClientSize.Width  / sizef.Width,
                                      ClientSize.Height / sizef.Height);
          Font     font    = new Font(Font.FontFamily,
                                      fScale * Font.SizeInPoints);

          StringFormat strfmt = new StringFormat();
          strfmt.Alignment = strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString(strTime, font, new SolidBrush(ForeColor), 
                          ClientRectangle, strfmt);
     }
}
