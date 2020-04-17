//----------------------------------------------------
// InstantiateHelloWorld.cs © 2001 by Charles Petzold
//----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class InstantiateHelloWorld
{
     public static void Main()
     {
          Form form   = new HelloWorld();
          form.Text   = "Instantiate " + form.Text;
          form.Paint += new PaintEventHandler(MyPaintHandler);

          Application.Run(form);
     }
     static void MyPaintHandler(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;

          grfx.DrawString("Hello from InstantiateHelloWorld!", 
                          form.Font, Brushes.Black, 0, 100);
     }
}