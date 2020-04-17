//------------------------------------------
// FourCorners.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FourCorners: Form
{
     public static void Main()
     {
          Application.Run(new FourCorners());
     }
     public FourCorners()
     {
          Text = "Four Corners Text Alignment";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
         ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics     grfx   = pea.Graphics;
          Brush        brush  = new SolidBrush(ForeColor);
          StringFormat strfmt = new StringFormat();

          strfmt.Alignment     = StringAlignment.Near;
          strfmt.LineAlignment = StringAlignment.Near;
          grfx.DrawString("Upper left corner", Font, brush, 0, 0, strfmt);

          strfmt.Alignment     = StringAlignment.Far;
          strfmt.LineAlignment = StringAlignment.Near;
          grfx.DrawString("Upper right corner", Font, brush, 
                          ClientSize.Width, 0, strfmt);

          strfmt.Alignment     = StringAlignment.Near;
          strfmt.LineAlignment = StringAlignment.Far;
          grfx.DrawString("Lower left corner", Font, brush, 
                          0, ClientSize.Height, strfmt);

          strfmt.Alignment     = StringAlignment.Far;
          strfmt.LineAlignment = StringAlignment.Far;
          grfx.DrawString("Lower right corner", Font, brush, 
                          ClientSize.Width, ClientSize.Height, strfmt);
     }
}