//---------------------------------------
// TwoForms.cs ?2001 by Charles Petzold
//---------------------------------------
using System.Windows.Forms;

class TwoForms
{
     public static void Main()
     {
          Form form1 = new Form();
          Form form2 = new Form();

          form1.Text = "Form passed to Run()";
          form2.Text = "Second form";
          form2.Show();

          Application.Run(form1);

          MessageBox.Show("Application.Run() has returned " +
                          "control back to Main. Bye, bye!",
                          "TwoForms");
     }
}
