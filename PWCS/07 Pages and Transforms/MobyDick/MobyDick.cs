//---------------------------------------
// MobyDick.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

class MobyDick: PrintableForm
{
     public new static void Main()
     {
          Application.Run(new MobyDick());
     }
     public MobyDick()
     {
          Text = "Moby-Dick by Herman Melville";
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
               // Insert RotateTransform, ScaleTransform, 
               //   TranslateTransform, and other calls here.

          grfx.DrawString("Call me Ishmael. Some years ago\x2014never " +
                          "mind how long precisely\x2014having little " +
                          "or no money in my purse, and nothing " +
                          "particular to interest me on shore, I " +
                          "thought I would sail about a little and " +
                          "see the watery part of the world. It is " +
                          "a way I have of driving off the spleen, " +
                          "and regulating the circulation. Whenever " +
                          "I find myself growing grim about the " +
                          "mouth; whenever it is a damp, drizzly " +
                          "November in my soul; whenever I find " +
                          "myself involuntarily pausing before " +
                          "coffin warehouses, and bringing up the " +
                          "rear of every funeral I meet; and " +
                          "especially whenever my hypos get such an " +
                          "upper hand of me, that it requires a " +
                          "strong moral principle to prevent me " +
                          "from deliberately stepping into the " +
                          "street, and methodically knocking " +
                          "people's hats off\x2014then, I account it " +
                          "high time to get to sea as soon as I " +
                          "can. This is my substitute for pistol " +
                          "and ball. With a philosophical flourish " +
                          "Cato throws himself upon his sword; I " +
                          "quietly take to the ship. There is " +
                          "nothing surprising in this. If they but " +
                          "knew it, almost all men in their degree, " +
                          "some time or other, cherish very nearly " +
                          "the same feelings towards the ocean with " +
                          "me.", 
                          Font, new SolidBrush(clr), 
                          new Rectangle(0, 0, cx, cy));
     }
}
