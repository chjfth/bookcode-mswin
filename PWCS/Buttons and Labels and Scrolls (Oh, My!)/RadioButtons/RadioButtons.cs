//-------------------------------------------
// RadioButtons.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RadioButtons: Form
{
     bool  bFillEllipse;
     Color colorEllipse;

     static void Main()
     {
          Application.Run(new RadioButtons());
     }
     RadioButtons()
     {
          Text = "Radio Buttons Demo";
          ResizeRedraw = true;

          string[] astrColor = { "Black", "Blue", "Green", "Cyan",   
                                 "Red", "Magenta", "Yellow", "White"};

          GroupBox grpbox = new GroupBox();
          grpbox.Parent   = this;
          grpbox.Text     = "Color";
          grpbox.Location = new Point(Font.Height / 2, Font.Height / 2);
          grpbox.Size     = new Size(9 * Font.Height, 
                              (3 * astrColor.Length + 4) * Font.Height / 2);

          for (int i = 0; i < astrColor.Length; i++)
          {
               RadioButton radiobtn = new RadioButton();
               radiobtn.Parent      = grpbox;
               radiobtn.Text        = astrColor[i];
               radiobtn.Location    = new Point(Font.Height,
                                             3 * (i + 1) * Font.Height / 2);
               radiobtn.Size        = new Size(7 * Font.Height, 
                                               3 * Font.Height / 2);
               radiobtn.CheckedChanged += 
                              new EventHandler(RadioButtonOnCheckedChanged);
               if(i == 0)
                    radiobtn.Checked = true;
          }
          CheckBox chkbox = new CheckBox();
          chkbox.Parent   = this;
          chkbox.Text     = "Fill Ellipse";
          chkbox.Location = new Point(Font.Height, 
                              3 * (astrColor.Length + 2) * Font.Height / 2);
          chkbox.Size     = new Size(Font.Height * 7, 3 * Font.Height / 2);
          chkbox.CheckedChanged += 
                         new EventHandler(CheckBoxOnCheckedChanged);
     }
     void RadioButtonOnCheckedChanged(object obj, EventArgs ea)
     {
          RadioButton radiobtn = (RadioButton) obj;

          if(radiobtn.Checked)
          {
               colorEllipse = Color.FromName(radiobtn.Text);
               Invalidate(false);
          }
     }
     void CheckBoxOnCheckedChanged(object obj, EventArgs ea)
     {
          bFillEllipse = ((CheckBox)obj).Checked;
          Invalidate(false);
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics  grfx = pea.Graphics;
          Rectangle rect = new Rectangle(10 * Font.Height, 0,
                                         ClientSize.Width - 
                                             10 * Font.Height - 1,
                                         ClientSize.Height - 1);
          if(bFillEllipse)
               grfx.FillEllipse(new SolidBrush(colorEllipse), rect);
          else
               grfx.DrawEllipse(new Pen(colorEllipse), rect);
     }
}
