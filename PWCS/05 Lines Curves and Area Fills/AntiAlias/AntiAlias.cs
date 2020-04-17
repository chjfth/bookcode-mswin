//----------------------------------------
// AntiAlias.cs © 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class AntiAlias: Form
{
     public static void Main()
     {
          Application.Run(new AntiAlias());
     }
     public AntiAlias()
     {
          Text = "Anti-Alias Demo";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          grfx.SmoothingMode   = SmoothingMode.None;
          grfx.PixelOffsetMode = PixelOffsetMode.Default;

          grfx.DrawLine(pen, 2, 2, 18, 10);
     }
}
