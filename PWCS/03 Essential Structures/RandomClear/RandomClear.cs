//------------------------------------------
// RandomClear.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class RandomClear: Form 
{
     public static void Main()
     {
          Application.Run(new RandomClear());
     }
     public RandomClear()      
     {
          Text = "Random Clear";
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