//-----------------------------------------------
// ShowFormAndSleep.cs © 2001 by Charles Petzold
//-----------------------------------------------
using System.Threading;
using System.Windows.Forms;

class ShowFormAndSleep
{
     public static void Main()
     {
          Form form = new Form();

          form.Show();

          Thread.Sleep(2500);

          form.Text = "My First Form";

          Thread.Sleep(2500);
     }
}