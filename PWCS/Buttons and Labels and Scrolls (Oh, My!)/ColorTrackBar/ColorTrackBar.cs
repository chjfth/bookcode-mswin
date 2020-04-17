//--------------------------------------------
// ColorTrackBar.cs © 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ColorTrackBar: Form
{
     Panel      panel;
     Label[]    alabelName  = new Label[3];
     Label[]    alabelValue = new Label[3];
     TrackBar[] atrackbar   = new TrackBar[3];

     public static void Main()
     {
          Application.Run(new ColorTrackBar());
     }
     public ColorTrackBar()
     {
          Text = "Color Track Bar";

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

               atrackbar[i] = new TrackBar();
               atrackbar[i].Parent = panel;
               atrackbar[i].Orientation = Orientation.Vertical;
               atrackbar[i].BackColor = acolor[i];
               atrackbar[i].SmallChange = 1;
               atrackbar[i].LargeChange = 16;
               atrackbar[i].Minimum = 0;
               atrackbar[i].Maximum = 255;
               atrackbar[i].TickFrequency = 16;
               atrackbar[i].ValueChanged += 
                              new EventHandler(TrackBarOnValueChanged);

               alabelValue[i] = new Label();
               alabelValue[i].Parent = panel;
               alabelValue[i].TextAlign = ContentAlignment.MiddleCenter;
          }
          Color color = BackColor;
          atrackbar[0].Value = color.R; // Generates ValueChanged event.
          atrackbar[1].Value = color.G;
          atrackbar[2].Value = color.B;

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

               atrackbar[i].Height = cy - 4 * cyFont;
               atrackbar[i].Location = 
                    new Point((1 + 2 * i) * cx / 12 - atrackbar[i].Width / 2,
                              2 * cyFont);

               alabelValue[i].Location = new Point(i * cx / 6,
                                                   cy - 3 * cyFont / 2);
               alabelValue[i].Size = new Size(cx / 6, cyFont);
          }
     }
     void TrackBarOnValueChanged(object obj, EventArgs ea)
     {
          for (int i = 0; i < 3; i++)
               if((TrackBar) obj == atrackbar[i])
                    alabelValue[i].Text = atrackbar[i].Value.ToString();

          BackColor = Color.FromArgb(atrackbar[0].Value, 
                                     atrackbar[1].Value,
                                     atrackbar[2].Value);
     }
}