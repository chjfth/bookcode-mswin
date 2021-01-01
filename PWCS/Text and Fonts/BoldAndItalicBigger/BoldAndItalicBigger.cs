//--------------------------------------------------
// BoldAndItalicBigger.cs (R) 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BoldAndItalicBigger: BoldAndItalic
{
     public new static void Main()
     {
          Application.Run(new BoldAndItalicBigger());
     }
     public BoldAndItalicBigger()
     {
          Text += " Bigger";
          Font = new Font("Times New Roman", 24);
     }
}
