//-------------------------------------------
// GradientText.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class GradientText: FontMenuForm
{
     public new static void Main()
     {
          Application.Run(new GradientText());
     }
     public GradientText()
     {
          Text = "Gradient Text";
          Width *= 3;
          strText = "Gradient";
          font = new Font("Times New Roman", 144, FontStyle.Italic);
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          SizeF  sizef  = grfx.MeasureString(strText, font);
          PointF ptf    = new PointF((cx - sizef.Width) / 2,
                                     (cy - sizef.Height) / 2);

          RectangleF rectf = new RectangleF(ptf, sizef);

          LinearGradientBrush lgbrush = new LinearGradientBrush(rectf, 
                                        Color.White, Color.Black, 
                                        LinearGradientMode.ForwardDiagonal);
          grfx.Clear(Color.Gray);
          grfx.DrawString(strText, font, lgbrush, ptf);
     }
}
