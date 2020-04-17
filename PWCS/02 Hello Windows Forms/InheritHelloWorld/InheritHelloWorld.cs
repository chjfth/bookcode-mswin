//------------------------------------------------
// InheritHelloWorld.cs ?2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class InheritHelloWorld: HelloWorld
{
     public new static void Main()
     {
          Application.Run(new InheritHelloWorld());
     }
     public InheritHelloWorld()
     {
          Text = "Inherit " + Text;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.DrawString("Hello from InheritHelloWorld!", 
                          Font, Brushes.Black, 0, 100);
     }
}