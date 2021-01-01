//--------------------------------------------------------
// HuckleberryFinnHalfHeight.cs (R) 2001 by Charles Petzold
//--------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HuckleberryFinnHalfHeight: Form
{
     public static void Main()
     {
          Application.Run(new HuckleberryFinnHalfHeight());
     }
     public HuckleberryFinnHalfHeight()
     {
          Text = "\"The Adventures of Huckleberry Finn\"";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics  grfx = pea.Graphics;
          int       cx   = ClientSize.Width;
          int       cy   = ClientSize.Height;
          Pen       pen  = new Pen(ForeColor);
          Rectangle rect = new Rectangle(0, 0, cx / 2, cy / 2);

          grfx.DrawString(     
               "You don't know about me, without you " +
               "have read a book by the name of \"The " +
               "Adventures of Tom Sawyer,\" but that " +
               "ain't no matter. That book was made by " +
               "Mr. Mark Twain, and he told the truth, " +
               "mainly. There was things which he " +
               "stretched, but mainly he told the truth. " +
               "That is nothing. I never seen anybody " +
               "but lied, one time or another, without " +
               "it was Aunt Polly, or the widow, or " +
               "maybe Mary. Aunt Polly\x2014Tom's Aunt " +
               "Polly, she is\x2014and Mary, and the Widow " +
               "Douglas, is all told about in that book" +
               "\x2014which is mostly a true book; with " +
               "some stretchers, as I said before.", 
               Font, new SolidBrush(ForeColor), rect);

          grfx.DrawLine(pen, 0,      cy / 2, cx / 2, cy / 2);
          grfx.DrawLine(pen, cx / 2, 0,      cx / 2, cy / 2);
     }
}
