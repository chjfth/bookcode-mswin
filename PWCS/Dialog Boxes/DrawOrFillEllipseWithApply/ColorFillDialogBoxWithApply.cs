//----------------------------------------------------------
// ColorFillDialogBoxWithApply.cs © 2001 by Charles Petzold
//----------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ColorFillDialogBoxWithApply: ColorFillDialogBox
{
     Button btnApply;      

     public event EventHandler Apply;

     public ColorFillDialogBoxWithApply()
     {
          grpbox.Location = new Point(36, 8);
          chkbox.Location = new Point(36, grpbox.Bottom + 4);

          btnApply = new Button();
          btnApply.Parent   = this;
          btnApply.Enabled  = false;
          btnApply.Text     = "Apply";
          btnApply.Location = new Point(120, chkbox.Bottom + 4);
          btnApply.Size     = new Size(40, 16);
          btnApply.Click   += new EventHandler(ButtonApplyOnClick);

          ClientSize = new Size(168, btnApply.Bottom + 8);
          AutoScaleBaseSize = new Size(4, 8);
     }
     public bool ShowApply
     {
          get { return btnApply.Enabled; }
          set { btnApply.Enabled = value; }
     }
     void ButtonApplyOnClick(object obj, EventArgs ea)
     {
          if (Apply != null)
               Apply(this, new EventArgs());
     }
}