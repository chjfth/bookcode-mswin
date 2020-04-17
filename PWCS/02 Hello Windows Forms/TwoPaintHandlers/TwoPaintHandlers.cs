//-----------------------------------------------
// TwoPaintHandlers.cs ?2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwoPaintHandlers
{
     public static void Main()
     {
          Form form      = new Form();
          form.Text      = "Two Paint Handlers";
          form.BackColor = Color.White;
          form.Paint    += new PaintEventHandler(PaintHandler1);
          form.Paint    += new PaintEventHandler(PaintHandler2);

          Application.Run(form);
     }
     static void PaintHandler1(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;

          grfx.DrawString("First Paint Event Handler", form.Font, 
                          Brushes.Black, 0, 0);
     }
     static void PaintHandler2(object objSender, PaintEventArgs pea)
     {
          Form     form = (Form)objSender;
          Graphics grfx = pea.Graphics;

          grfx.DrawString("Second Paint Event Handler", form.Font, 
                          Brushes.Black, 0, 100);
     }
}
