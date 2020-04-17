//-------------------------------------------
// RunFormBadly.cs © 2001 by Charles Petzold
//-------------------------------------------
using System.Windows.Forms;

class RunFormBadly
{
     public static void Main()
     {
          Form form = new Form();

          form.Text = "Not a Good Idea...";
          form.Visible = true;

          Application.Run();
     }
}
