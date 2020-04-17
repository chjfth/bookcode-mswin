//------------------------------------------------------
// RandomClearResizeRedraw.cs © 2001 by Charles Petzold
//------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RandomClearResizeRedraw: Form
{
     public static void Main()      
     {
          Application.Run(new RandomClearResizeRedraw());
     }
     public RandomClearResizeRedraw()      
     {
          Text = "Random Clear with Resize Redraw";
          ResizeRedraw = true;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Random   rand = new Random();

          grfx.Clear(Color.FromArgb(rand.Next(256),
                                    rand.Next(256),
                                    rand.Next(256)));
     }
}