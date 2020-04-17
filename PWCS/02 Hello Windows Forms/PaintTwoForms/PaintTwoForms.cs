//--------------------------------------------
// PaintTwoForms.cs ?2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PaintTwoForms
{
     static Form form1, form2;

     public static void Main()
     {
          form1 = new Form();
          form2 = new Form();

          form1.Text      = "First Form";
          form1.BackColor = Color.White;
          form1.Paint    += new PaintEventHandler(MyPaintHandler);

          form2.Text      = "Second Form";
          form2.BackColor = Color.White;
          form2.Paint    += new PaintEventHandler(MyPaintHandler);
          form2.Show();

          Application.Run(form1);
     }
     static void MyPaintHandler(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;
          string   str;

          if(form == form1)
               str = "Hello from the first form";
          else
               str = "Hello from the second form";

          grfx.DrawString(str, form.Font, Brushes.Black, 0, 0);
     }
}
