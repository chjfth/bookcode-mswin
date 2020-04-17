//-------------------------------------------------
// ColorFillDialogBox.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ColorFillDialogBox: Form
{
     protected GroupBox grpbox;
     protected CheckBox chkbox;

     public ColorFillDialogBox()
     {
          Text = "Color/Fill Select";

          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MinimizeBox     = false;
          MaximizeBox     = false;
          ShowInTaskbar   = false;
          Location        = ActiveForm.Location + 
                            SystemInformation.CaptionButtonSize +
                            SystemInformation.FrameBorderSize;

          string[] astrColor = { "Black", "Blue", "Green", "Cyan",   
                                 "Red", "Magenta", "Yellow", "White"};

          grpbox = new GroupBox();
          grpbox.Parent   = this;
          grpbox.Text     = "Color";
          grpbox.Location = new Point(8, 8);
          grpbox.Size     = new Size(96, 12 * (astrColor.Length + 1));

          for (int i = 0; i < astrColor.Length; i++)
          {
               RadioButton radiobtn = new RadioButton();
               radiobtn.Parent      = grpbox;
               radiobtn.Text        = astrColor[i];
               radiobtn.Location    = new Point(8, 12 * (i + 1));
               radiobtn.Size        = new Size(80, 10);
          }
          chkbox = new CheckBox();
          chkbox.Parent   = this;
          chkbox.Text     = "Fill Ellipse";
          chkbox.Location = new Point(8, grpbox.Bottom + 4);
          chkbox.Size     = new Size(80, 10);

          Button btn   = new Button();
          btn.Parent   = this;
          btn.Text     = "OK";
          btn.Location = new Point(8, chkbox.Bottom + 4);
          btn.Size     = new Size(40, 16);
          btn.DialogResult = DialogResult.OK;
          AcceptButton = btn;

          btn  = new Button();
          btn.Parent   = this;
          btn.Text     = "Cancel";
          btn.Location = new Point(64, chkbox.Bottom + 4);
          btn.Size     = new Size(40, 16);
          btn.DialogResult = DialogResult.Cancel;
          CancelButton = btn;

          ClientSize = new Size(112, btn.Bottom + 8);
          AutoScaleBaseSize = new Size(4, 8);
     }
     public Color Color
     {
          get 
          { 
               for (int i = 0; i < grpbox.Controls.Count; i++)
               {
                    RadioButton radiobtn = (RadioButton) grpbox.Controls[i];

                    if (radiobtn.Checked)
                         return Color.FromName(radiobtn.Text);
               }
               return Color.Black;
               
          }  
          set 
          { 
               for (int i = 0; i < grpbox.Controls.Count; i++)
               {
                    RadioButton radiobtn = (RadioButton) grpbox.Controls[i];

                    if (value == Color.FromName(radiobtn.Text))
                    {
                         radiobtn.Checked = true;
                         break;
                    }
               }
          }
     }
     public bool Fill
     {
          get { return chkbox.Checked; }
          set { chkbox.Checked = value; }
     }
}
