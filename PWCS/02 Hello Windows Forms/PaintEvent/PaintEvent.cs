//-----------------------------------------
// PaintEvent.cs (R) 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PaintEvent
{
     public static void Main()
     {
          Form form   = new Form();
          form.Text   = "Paint Event";
          form.Paint += new PaintEventHandler(MyPaintHandler);

          Application.Run(form);
     }
     static void MyPaintHandler(object objSender, PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.Clear(Color.Chocolate);
     }
}