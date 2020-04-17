//----------------------------------------------
// HuckleberryFinn.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HuckleberryFinn: Form
{
     public static void Main() 
     {
          Application.Run(new HuckleberryFinn()); 
     }
     public HuckleberryFinn()
     {
          Text = "\"The Adventures of Huckleberry Finn\"";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          pea.Graphics.DrawString(     
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
               Font, new SolidBrush(ForeColor), ClientRectangle);
     }
}