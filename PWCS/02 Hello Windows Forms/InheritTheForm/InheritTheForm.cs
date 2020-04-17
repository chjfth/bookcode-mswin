//---------------------------------------------
// InheritTheForm.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class InheritTheForm: Form
{
     public static void Main()
     {
          InheritTheForm form = new InheritTheForm();
          form.Text = "Inherit the Form";
          form.BackColor = Color.White;

          Application.Run(form);
     }
}