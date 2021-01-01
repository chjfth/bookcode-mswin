//---------------------------------------------
// TwoButtonsDock.cs (R) 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwoButtonsDock: Form
{
     public static void Main()
     {
          Application.Run(new TwoButtonsDock());
     }
     public TwoButtonsDock()
     {
          Text = "Two Buttons with Dock";
          ResizeRedraw = true;

          Button btn = new Button();
          btn.Parent = this;
          btn.Text   = "&Larger";
          btn.Height = 2 * Font.Height;
          btn.Dock   = DockStyle.Top;
          btn.Click += new EventHandler(ButtonLargerOnClick);
          
          btn = new Button();
          btn.Parent = this;
          btn.Text   = "&Smaller";
          btn.Height = 2 * Font.Height;
          btn.Dock   = DockStyle.Bottom;
          btn.Click += new EventHandler(ButtonSmallerOnClick);
     }
     void ButtonLargerOnClick(object obj, EventArgs ea)
     {
          Left   -= (int)(0.05 * Width);
          Top    -= (int)(0.05 * Height);
          Width  += (int)(0.10 * Width);
          Height += (int)(0.10 * Height);
     }
     void ButtonSmallerOnClick(object obj, EventArgs ea)
     {
          Left   += (int)(Width  / 22f);
          Top    += (int)(Height / 22f);
          Width  -= (int)(Width  / 11f);
          Height -= (int)(Height / 11f);
     }
}
