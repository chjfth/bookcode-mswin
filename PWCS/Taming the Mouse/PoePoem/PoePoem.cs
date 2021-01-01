//--------------------------------------
// PoePoem.cs (R) 2001 by Charles Petzold
//--------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class PoePoem: Form
{
     const string strAnnabelLee =

          "It was many and many a year ago,\n"                         +
          "   In a kingdom by the sea,\n"                              +
          "That a maiden there lived whom you may know\n"              +
          "   By the name of Annabel Lee;\x2014\n"                     +
          "And this maiden she lived with no other thought\n"          +
          "   Than to love and be loved by me.\n"                      +
          "\n"                                                         +
          "I was a child and she was a child\n"                        +
          "   In this kingdom by the sea,\n"                           +
          "But we loved with a love that was more than love\x2014\n"   +
          "   I and my Annabel Lee\x2014\n"                            +
          "With a love that the wingéd seraphs of Heaven\n"            +
          "   Coveted her and me.\n"                                   +
          "\n"                                                         +
          "And this was the reason that, long ago,\n"                  +
          "   In this kingdom by the sea,\n"                           +
          "A wind blew out of a cloud, chilling\n"                     +
          "   My beautiful Annabel Lee;\n"                             +
          "So that her highborn kinsmen came\n"                        +
          "   And bore her away from me,\n"                            +
          "To shut her up in a sepulchre,\n"                           +
          "   In this kingdom by the sea.\n"                           +
          "\n"                                                         +
          "The angels, not half so happy in Heaven,\n"                 +
          "   Went envying her and me\x2014\n"                         +
          "Yes! that was the reason (as all men know,\n"               +
          "   In this kingdom by the sea)\n"                           +
          "That the wind came out of the cloud by night,\n"            +
          "   Chilling and killing my Annabel Lee.\n"                  +
          "\n"                                                         +
          "But our love it was stronger by far than the love\n"        +
          "   Of those who were older than we\x2014\n"                 +
          "   Of many far wiser than we\x2014\n"                       +
          "And neither the angels in Heaven above\n"                   +
          "   Nor the demons down under the sea\n"                     +
          "Can ever dissever my soul from the soul\n"                  +
          "   Of the beautiful Annabel Lee:\x2014\n"                   +
          "\n"                                                         +
          "For the moon never beams, without bringing me dreams\n"     +
          "   Of the beautiful Annabel Lee;\n"                         +
          "And the stars never rise, but I feel the bright eyes\n"     +
          "   Of the beautiful Annabel Lee:\x2014\n"                   +
          "And so, all the night-tide, I lie down by the side\n"       +
          "Of my darling\x2014my darling\x2014my life and my bride,\n" +
          "   In her sepulchre there by the sea\x2014\n"               +
          "   In her tomb by the sounding sea.\n"                      +
          "\n"                                                         +
          "                                       [May 1849]\n";

     readonly int   iTextLines = 0;
     int            iClientLines, iStartLine = 0;
     float          cyText;

     public static void Main()
     {
               // See whether the program makes sense.

          if (!SystemInformation.MouseWheelPresent)
          {
               MessageBox.Show("Program needs a mouse with a mouse wheel!",
                               "PoePoem", MessageBoxButtons.OK,
                               MessageBoxIcon.Error);
               return;
          }
               // Otherwise go normally.

          Application.Run(new PoePoem());
     }
     public PoePoem()
     {
          Text = "\"Annabel Lee\" by Edgar Allan Poe";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;

               // Calculate the number of lines in the text.

          int iIndex = 0;

          while((iIndex = strAnnabelLee.IndexOf('\n', iIndex)) != -1)
          {
               iTextLines++;
               iIndex++;
          }
				// Obtain line-spacing value.

		 Graphics grfx = CreateGraphics();
		 cyText = Font.GetHeight(grfx);
		 grfx.Dispose();

		 OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          iClientLines = (int) (ClientSize.Height / cyText);

          iStartLine = Math.Max(0, 
                       Math.Min(iStartLine, iTextLines - iClientLines));
     }
     protected override void OnMouseWheel(MouseEventArgs mea)
     {
          int iScroll = 
               mea.Delta * SystemInformation.MouseWheelScrollLines / 120;

          iStartLine -= iScroll;
          iStartLine  = Math.Max(0, 
                        Math.Min(iStartLine, iTextLines - iClientLines));
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          grfx.DrawString(strAnnabelLee, Font, new SolidBrush(ForeColor),
                          0, -iStartLine * cyText);
     }
}
