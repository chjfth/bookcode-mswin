//-------------------------------------------------
// NotepadCloneNoMenu.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class NotepadCloneNoMenu: Form
{
     protected TextBox txtbox;

     public static void Main()
     {
          Application.Run(new NotepadCloneNoMenu());
     }
     public NotepadCloneNoMenu()
     {
          Text = "Notepad Clone No Menu";

          txtbox             = new TextBox();
          txtbox.Parent      = this;
          txtbox.Dock        = DockStyle.Fill;
          txtbox.BorderStyle = BorderStyle.None;
          txtbox.Multiline   = true;
          txtbox.ScrollBars  = ScrollBars.Both;
          txtbox.AcceptsTab  = true;
     }
}          
