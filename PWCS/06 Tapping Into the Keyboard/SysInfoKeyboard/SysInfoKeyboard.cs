//----------------------------------------------
// SysInfoKeyboard.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoKeyboard: SysInfoReflection
{
     public new static void Main()
     {
          Application.Run(new SysInfoKeyboard());
     }
     public SysInfoKeyboard()
     {
          Text = "System Information: Keyboard";
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          Point pt = AutoScrollPosition;

          pt.X = -pt.X;
          pt.Y = -pt.Y;

          switch(kea.KeyCode)
          {
          case Keys.Right:
               if ((kea.Modifiers & Keys.Control) == Keys.Control)
                    pt.X += ClientSize.Width;
               else
                    pt.X += Font.Height;
               break;

          case Keys.Left:      
               if ((kea.Modifiers & Keys.Control) == Keys.Control)
                    pt.X -= ClientSize.Width;
               else
                    pt.X -= Font.Height;
               break;
               
          case Keys.Down:      pt.Y += Font.Height;        break;
          case Keys.Up:        pt.Y -= Font.Height;        break;
          case Keys.PageDown:  
               pt.Y += Font.Height * (ClientSize.Height / Font.Height);  
               break;
          case Keys.PageUp:    
			   pt.Y -= Font.Height * (ClientSize.Height / Font.Height);
			   break;
          case Keys.Home:      pt    = Point.Empty;        break;
          case Keys.End:       pt.Y  = 1000000;            break;
          }
          AutoScrollPosition = pt;
     }
}
