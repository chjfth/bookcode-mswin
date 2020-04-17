//--------------------------------------------------
// BetterImageFromFile.cs © 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterImageFromFile: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new BetterImageFromFile());
     }
     public BetterImageFromFile()
     {
          Text = "Better Image From File";

          string strFileName = "..\\..\\..\\Apollo11FullColor.jpg";

          try
          {
               image = Image.FromFile(strFileName);
          }
          catch
          {
               MessageBox.Show("Cannot find file " + strFileName + "!",
                         Text, MessageBoxButtons.OK, MessageBoxIcon.Hand);
          }
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          if (image == null)
               return;

          grfx.DrawImage(image, 0, 0);
     }
}
