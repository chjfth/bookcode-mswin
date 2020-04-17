//---------------------------------------------
// MatrixElements.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class MatrixElements: Form
{
     Matrix          matrix; 
     Button          btnUpdate;
     NumericUpDown[] updown = new NumericUpDown[6];

     public event EventHandler Changed;

     public MatrixElements() 
     {
          Text = "Matrix Elements";
          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MinimizeBox     = false;
          MaximizeBox     = false;
          ShowInTaskbar   = false;

          String[] strLabel = { "X Scale:",     "Y Shear:",
                                "X Shear:",     "Y Scale:",
                                "X Translate:", "Y Translate:" };

          for (int i = 0; i < 6; i++)
          {
               Label label    = new Label();
               label.Parent   = this;
               label.Text     = strLabel[i];
               label.Location = new Point(8, 8 + 16 * i);
               label.Size     = new Size(64, 8);

               updown[i] = new NumericUpDown();
               updown[i].Parent        = this;
               updown[i].Location      = new Point(76, 8 + 16 * i);
               updown[i].Size          = new Size(48, 12);
               updown[i].TextAlign     = HorizontalAlignment.Right;
               updown[i].ValueChanged += new EventHandler
                                                  (UpDownOnValueChanged);
               updown[i].DecimalPlaces = 2;
               updown[i].Increment = 0.1m;
               updown[i].Minimum = Decimal.MinValue;
               updown[i].Maximum = Decimal.MaxValue;
          }
          btnUpdate          = new Button();
          btnUpdate.Parent   = this;
          btnUpdate.Text     = "Update";
          btnUpdate.Location = new Point(8, 108);
          btnUpdate.Size     = new Size (50, 16);
          btnUpdate.Click   += new EventHandler(ButtonUpdateOnClick);

          AcceptButton = btnUpdate;

          Button btn   = new Button();
          btn.Parent   = this;
          btn.Text     = "Methods...";
          btn.Location = new Point(76, 108);
          btn.Size     = new Size(50, 16);
          btn.Click   += new EventHandler(ButtonMethodsOnClick);

          ClientSize = new Size(134, 132);

          AutoScaleBaseSize = new Size(4, 8);
     }
     public Matrix Matrix 
     {
          get 
          { 
               matrix = new Matrix((float) updown[0].Value,
                                   (float) updown[1].Value,
                                   (float) updown[2].Value,
                                   (float) updown[3].Value,
                                   (float) updown[4].Value,
                                   (float) updown[5].Value);
               return matrix; 
          } 
          set 
          { 
               matrix = value;

               for (int i = 0; i < 6; i++)
                    updown[i].Value = (decimal) value.Elements[i];
          } 
     }
     void UpDownOnValueChanged(object obj, EventArgs ea)
     {
          Graphics grfx = CreateGraphics();

          bool boolEnableButton = true;

          try
          {
               grfx.Transform = Matrix;
          }
          catch
          {
               boolEnableButton = false;
          }
          btnUpdate.Enabled = boolEnableButton;          
          grfx.Dispose();
     }
     void ButtonUpdateOnClick(object obj, EventArgs ea)
     {
          if (Changed != null)
               Changed(this, new EventArgs());
     }
     void ButtonMethodsOnClick(object obj, EventArgs ea)
     {
          MatrixMethods dlg = new MatrixMethods();

          dlg.Matrix = Matrix;

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               Matrix = dlg.Matrix;
               btnUpdate.PerformClick();
          }
     }
}