//--------------------------------------------
// RunFormBetter.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System.Windows.Forms;

class RunFormBetter
{
     public static void Main()
     {
          Form form = new Form();

          form.Text = "My Very Own Form";

          Application.Run(form);
     }
}
