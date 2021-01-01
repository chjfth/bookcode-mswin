//------------------------------------------
// ColorScroll.cs (R) 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ColorScroll: Form
{
     Panel        panel;
     Label[]      alabelName  = new Label[3];
     Label[]      alabelValue = new Label[3];
     VScrollBar[] avscroll    = new VScrollBar[3];

     public static void Main()
     {
          Application.Run(new ColorScroll());
     }
     public ColorScroll()
     {
          Text = "Color Scroll";

          Color[] acolor = { Color.Red, Color.Green, Color.Blue };

               // Create the panel.

          panel = new Panel();
          panel.Parent = this;
          panel.Location = new Point(0, 0);
          panel.BackColor = Color.White;

               // Loop through the three colors.

          for (int i = 0; i < 3; i++)
          {
               alabelName[i] = new Label();
               alabelName[i].Parent = panel;
               alabelName[i].ForeColor = acolor[i];
               alabelName[i].Text = "&" + acolor[i].ToKnownColor();
               alabelName[i].TextAlign = ContentAlignment.MiddleCenter;

               avscroll[i] = new VScrollBar();
               avscroll[i].Parent = panel;
               avscroll[i].SmallChange = 1;
               avscroll[i].LargeChange = 16;
               avscroll[i].Minimum  = 0;
               avscroll[i].Maximum = 255 + avscroll[i].LargeChange - 1;
               avscroll[i].ValueChanged += 
                              new EventHandler(ScrollOnValueChanged);
               avscroll[i].TabStop = true;

               alabelValue[i] = new Label();
               alabelValue[i].Parent = panel;
               alabelValue[i].TextAlign = ContentAlignment.MiddleCenter;
          }
          Color color = BackColor;
          avscroll[0].Value = color.R;  // Generates ValueChanged event.
          avscroll[1].Value = color.G;
          avscroll[2].Value = color.B;

		  OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          int cx = ClientSize.Width;
          int cy = ClientSize.Height;
          int cyFont = Font.Height;

          panel.Size = new Size(cx / 2, cy);

          for (int i = 0; i < 3; i++)
          {
               alabelName[i].Location = new Point(i * cx / 6, cyFont / 2);
               alabelName[i].Size = new Size(cx / 6, cyFont);

               avscroll[i].Location = new Point((4 * i + 1) * cx / 24,
                                                2 * cyFont);
               avscroll[i].Size = new Size(cx / 12, cy - 4 * cyFont);

               alabelValue[i].Location = new Point(i * cx / 6,
                                                   cy - 3 * cyFont / 2);
               alabelValue[i].Size = new Size(cx / 6, cyFont);
          }
     }
     void ScrollOnValueChanged(Object obj, EventArgs ea)
     {
          for (int i = 0; i < 3; i++)
               if((VScrollBar) obj == avscroll[i])
                    alabelValue[i].Text = avscroll[i].Value.ToString();

          BackColor = Color.FromArgb(avscroll[0].Value, 
                                     avscroll[1].Value,
                                     avscroll[2].Value);
     }
}