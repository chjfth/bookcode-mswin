//-----------------------------------------------------
// InheritWithConstructor.cs (R) 2001 by Charles Petzold
//-----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class InheritWithConstructor: Form
{
     public static void Main()
     {
          Application.Run(new InheritWithConstructor());
     }
     public InheritWithConstructor()
     {
          Text = "Inherit with Constructor";
          BackColor = Color.White;
     }
}