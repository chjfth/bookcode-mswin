//-------------------------------------------
// SimpleButton.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleButton: Form
{
     public static void Main()
     {
          Application.Run(new SimpleButton());
     }
     public SimpleButton()
     {
          Text = "Simple Button";

          Button btn   = new Button();
          btn.Parent   = this;
          btn.Text     = "Click Me!";
          btn.Location = new Point(100, 100);
          btn.Click   += new EventHandler(ButtonOnClick);
     }
     void ButtonOnClick(object obj, EventArgs ea)
     {
          Graphics grfx   = CreateGraphics();
          Point    ptText = Point.Empty;
          string   str    = "Button clicked!";

          grfx.DrawString(str, Font, new SolidBrush(ForeColor), ptText);
          System.Threading.Thread.Sleep(250);
          grfx.FillRectangle(new SolidBrush(BackColor), 
               new RectangleF(ptText, grfx.MeasureString(str, Font)));

          grfx.Dispose();
     }
}
