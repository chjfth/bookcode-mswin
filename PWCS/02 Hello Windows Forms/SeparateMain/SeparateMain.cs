//-------------------------------------------
// SeparateMain.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SeparateMain
{
     public static void Main()
     {
          Application.Run(new AnotherHelloWorld());
     }
}
class AnotherHelloWorld: Form
{
     public AnotherHelloWorld()
     {
          Text = "Another Hello World";
          BackColor = Color.White;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.DrawString("Hello, Windows Forms!", Font, Brushes.Black, 0, 0);
     }
}