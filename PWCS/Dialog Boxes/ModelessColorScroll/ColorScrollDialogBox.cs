//---------------------------------------------------
// ColorScrollDialogBox.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ColorScrollDialogBox: Form
{
     Label[]      alabelName  = new Label[3];
     Label[]      alabelValue = new Label[3];
     VScrollBar[] avscroll    = new VScrollBar[3];

     public event EventHandler Changed;

     public ColorScrollDialogBox()
     {
          Text = "Color Scroll Dialog Box";

          ControlBox    = false;
          MinimizeBox   = false;
          MaximizeBox   = false;
          ShowInTaskbar = false;

          Color[] acolor = { Color.Red, Color.Green, Color.Blue };

          for (int i = 0; i < 3; i++)
          {
               alabelName[i] = new Label();
               alabelName[i].Parent = this;
               alabelName[i].ForeColor = acolor[i];
               alabelName[i].Text = "&" + acolor[i].ToKnownColor();
               alabelName[i].TextAlign = ContentAlignment.MiddleCenter;

               avscroll[i] = new VScrollBar();
               avscroll[i].Parent = this;
               avscroll[i].SmallChange = 1;
               avscroll[i].LargeChange = 16;
               avscroll[i].Minimum  = 0;
               avscroll[i].Maximum = 255 + avscroll[i].LargeChange - 1;
               avscroll[i].ValueChanged += 
                              new EventHandler(ScrollOnValueChanged);
               avscroll[i].TabStop = true;

               alabelValue[i] = new Label();
               alabelValue[i].Parent = this;
               alabelValue[i].TextAlign = ContentAlignment.MiddleCenter;
          }

		  OnResize(EventArgs.Empty);
     }
     public Color Color
     {
          get 
          { 
               return Color.FromArgb(avscroll[0].Value,
                                     avscroll[1].Value,
                                     avscroll[2].Value); 
          }
          set 
          {
               avscroll[0].Value = value.R;
               avscroll[1].Value = value.G;
               avscroll[2].Value = value.B;
          }
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          int cx = ClientSize.Width;
          int cy = ClientSize.Height;
          int cyFont = Font.Height;

          for (int i = 0; i < 3; i++)
          {
               alabelName[i].Location = new Point(i * cx / 3, cyFont / 2);
               alabelName[i].Size = new Size(cx / 3, cyFont);

               avscroll[i].Location = new Point((4 * i + 1) * cx / 12,
                                                2 * cyFont);
               avscroll[i].Size = new Size(cx / 6, cy - 4 * cyFont);

               alabelValue[i].Location = new Point(i * cx / 3,
                                                   cy - 3 * cyFont / 2);
               alabelValue[i].Size = new Size(cx / 3, cyFont);
          }
     }
     void ScrollOnValueChanged(Object obj, EventArgs ea)
     {
          for (int i = 0; i < 3; i++)
               if((VScrollBar) obj == avscroll[i])
                    alabelValue[i].Text = avscroll[i].Value.ToString();

          if (Changed != null)
               Changed(this, new EventArgs());
     }
}