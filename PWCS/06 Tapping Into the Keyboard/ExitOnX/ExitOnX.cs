//--------------------------------------
// ExitOnX.cs (R) 2001 by Charles Petzold
//--------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ExitOnX: Form
{
     public static void Main()
     {
          Application.Run(new ExitOnX());
     }
     public ExitOnX()
     {
          Text = "Exit on X";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          if (kea.KeyCode == Keys.X)
               Close();
     }
}
