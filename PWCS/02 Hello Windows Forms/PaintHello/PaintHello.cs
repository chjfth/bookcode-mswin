//-----------------------------------------
// PaintHello.cs ?2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PaintHello
{
     public static void Main()
     {
          Form form      = new Form();
          form.Text      = "Paint Hello";
          form.BackColor = Color.White;
          
//		form.Paint    += new PaintEventHandler(MyPaintHandler);
		  form.Paint += MyPaintHandler;

          Application.Run(form);
     }
     static void MyPaintHandler(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;

		  Console.WriteLine("Paint...");
          grfx.DrawString("Hello, world!", form.Font, Brushes.Black, 0, 0);
     }
}