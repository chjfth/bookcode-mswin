//-----------------------------------------
// HelloWorld.cs ?2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HelloWorld: Form
{
     public static void Main()
     {
          Application.Run(new HelloWorld());
     }
     public HelloWorld()
     {
          Text = "Hello World";
          BackColor = Color.White;

		 Paint += MyPaintHandler;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.DrawString("Hello, Windows Forms!", Font, Brushes.Black, 0, 0);

		  //base.OnPaint(pea);
     }

     static void MyPaintHandler(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;

		  Console.WriteLine("Paint...");
          grfx.DrawString("CHJ ----- Hello, world!", form.Font, Brushes.Black, 0, 0);
     }
}