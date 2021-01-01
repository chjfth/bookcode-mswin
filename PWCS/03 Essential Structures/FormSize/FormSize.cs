//---------------------------------------
// FormSize.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FormSize: Form
{
     public static void Main()
     {
          Application.Run(new FormSize());
     }
     public FormSize()
     {
          Text = "Form Size";
          BackColor = Color.White;
     }
     protected override void OnMove(EventArgs ea)
     {
          Invalidate();
     }
     protected override void OnResize(EventArgs ea)
     {
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          string   str  = "Location: "        + Location        + "\n"   +
                          "Size: "            + Size            + "\n"   +
                          "Bounds: "          + Bounds          + "\n"   +
                          "Width: "           + Width           + "\n"   +
                          "Height: "          + Height          + "\n"   +
                          "Left: "            + Left            + "\n"   +
                          "Top: "             + Top             + "\n"   +
                          "Right: "           + Right           + "\n"   +
                          "Bottom: "          + Bottom          + "\n\n" +
                          "DesktopLocation: " + DesktopLocation + "\n"   +
                          "DesktopBounds: "   + DesktopBounds   + "\n\n" +
                          "ClientSize: "      + ClientSize      + "\n"   +
                          "ClientRectangle: " + ClientRectangle;

          grfx.DrawString(str, Font, Brushes.Black, 0, 0);
    }
}