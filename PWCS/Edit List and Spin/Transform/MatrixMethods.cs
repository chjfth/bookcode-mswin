//--------------------------------------------
// MatrixMethods.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class MatrixMethods: Form
{
     Matrix          matrix; 
     Button          btnInvert;
     NumericUpDown[] updown = new NumericUpDown[3];
     RadioButton[]   radio = new RadioButton[2];

     public MatrixMethods() 
     {
          Text = "Matrix Methods";
          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MinimizeBox     = false;
          MaximizeBox     = false;
          ShowInTaskbar   = false;
          Location        = ActiveForm.Location +
                            SystemInformation.CaptionButtonSize +
                            SystemInformation.FrameBorderSize;

          String[] astrLabel = { "X / DX:", "Y / DY:", "Angle:" };

          for (int i = 0; i < 3; i++)
          {
               Label label    = new Label();
               label.Parent   = this;
               label.Text     = astrLabel[i];
               label.Location = new Point(8, 8 + 16 * i);
               label.Size     = new Size(32, 8);

               updown[i] = new NumericUpDown();
               updown[i].Parent        = this;
               updown[i].Location      = new Point(40, 8 + 16 * i);
               updown[i].Size          = new Size(48, 12);
               updown[i].TextAlign     = HorizontalAlignment.Right;

               updown[i].DecimalPlaces = 2;
               updown[i].Increment = 0.1m;
               updown[i].Minimum = Decimal.MinValue;
               updown[i].Maximum = Decimal.MaxValue;
          }
               // Create group box and radio buttons.

          GroupBox grpbox = new GroupBox();
          grpbox.Parent   = this;
          grpbox.Text     = "Order";
          grpbox.Location = new Point(8, 60);
          grpbox.Size     = new Size(80, 32);

          for (int i = 0; i < 2; i++)
          {
               radio[i] = new RadioButton();
               radio[i].Parent = grpbox;
               radio[i].Text = new string[] { "Prepend", "Append" } [i];
               radio[i].Location = new Point(8, 8 + 12 * i);
               radio[i].Size     = new Size(50, 10);
               radio[i].Checked = (i == 0);
          }

               // Create 8 buttons for terminating dialog.

          string[] astrButton = { "Reset", "Invert", "Translate", "Scale",
                                  "Rotate", "RotateAt", "Shear", "Cancel" };

          EventHandler[] aeh = { new EventHandler(ButtonResetOnClick), 
                                 new EventHandler(ButtonInvertOnClick),
                                 new EventHandler(ButtonTranslateOnClick), 
                                 new EventHandler(ButtonScaleOnClick),
                                 new EventHandler(ButtonRotateOnClick), 
                                 new EventHandler(ButtonRotateAtOnClick),
                                 new EventHandler(ButtonShearOnClick) };

          for (int i = 0; i < 8; i++)
          {
               Button btn = new Button();
               btn.Parent = this;
               btn.Text   = astrButton[i];
               btn.Location = new Point(100 + 72 * (i > 3 ? 1 : 0), 
                                        8 + (i % 4) * 24);
               btn.Size     = new Size(64, 14);
     
               if (i == 0)    // Reset button
               {
                    AcceptButton = btn;
               }
               if (i == 1)    // Invert button
               {
                    btnInvert = btn;
               }
               if (i < 7)     // All buttons except Cancel
               {
                    btn.Click += aeh[i];
                    btn.DialogResult = DialogResult.OK;
               }
               else           // Cancel button
               {
                    btn.DialogResult = DialogResult.Cancel;
                    CancelButton = btn;
               }
          }
          ClientSize = new Size(240, 106);

          AutoScaleBaseSize = new Size(4, 8);
     }
     public Matrix Matrix 
     {
          get 
          { 
               return matrix; 
          } 
          set 
          { 
               matrix = value;
               btnInvert.Enabled = matrix.IsInvertible;
          } 
     }
     void ButtonResetOnClick(object obj, EventArgs ea)
     {
          matrix.Reset();
     }
     void ButtonInvertOnClick(object obj, EventArgs ea)
     {
          matrix.Invert();
     }
     void ButtonTranslateOnClick(object obj, EventArgs ea)
     {
          matrix.Translate((float) updown[0].Value, 
                           (float) updown[1].Value,
               radio[0].Checked ? MatrixOrder.Prepend : MatrixOrder.Append);
     }
     void ButtonScaleOnClick(object obj, EventArgs ea)
     {
          matrix.Scale((float) updown[0].Value, 
                       (float) updown[1].Value,
               radio[0].Checked ? MatrixOrder.Prepend : MatrixOrder.Append);
     }
     void ButtonRotateOnClick(object obj, EventArgs ea)
     {
          matrix.Rotate((float) updown[2].Value,
               radio[0].Checked ? MatrixOrder.Prepend : MatrixOrder.Append);
     }
     void ButtonRotateAtOnClick(object obj, EventArgs ea)
     {
          matrix.RotateAt((float) updown[2].Value, 
                          new PointF((float) updown[0].Value, 
                                     (float) updown[1].Value),
               radio[0].Checked ? MatrixOrder.Prepend : MatrixOrder.Append);
     }
     void ButtonShearOnClick(object obj, EventArgs ea)
     {
          matrix.Shear((float) updown[0].Value, 
                       (float) updown[1].Value,
               radio[0].Checked ? MatrixOrder.Prepend : MatrixOrder.Append);
     }
}