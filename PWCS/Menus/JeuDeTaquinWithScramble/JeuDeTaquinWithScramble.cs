//------------------------------------------------------
// JeuDeTaquinWithScramble.cs © 2001 by Charles Petzold
//------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class JeuDeTaquinWithScramble: JeuDeTaquin
{
     public new static void Main()
     {
          Application.Run(new JeuDeTaquinWithScramble());
     }
     public JeuDeTaquinWithScramble()
     {
          Menu = new MainMenu(new MenuItem[] { 
                    new MenuItem("&Scramble!", 
                         new EventHandler(MenuScrambleOnClick)) });
     }
     void MenuScrambleOnClick(object obj, EventArgs ea)
     {
          Randomize();
     }
}
