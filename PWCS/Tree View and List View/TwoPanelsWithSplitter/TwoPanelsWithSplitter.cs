//----------------------------------------------------
// TwoPanelsWithSplitter.cs © 2001 by Charles Petzold
//----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwoPanelsWithSplitter: Form
{
     public static void Main()
     {
          Application.Run(new TwoPanelsWithSplitter());
     }
     public TwoPanelsWithSplitter()
     {
          Text = "Two Panels with Splitter";

          Panel panel1     = new Panel();
          panel1.Parent    = this;
          panel1.Dock      = DockStyle.Fill;
          panel1.BackColor = Color.Lime;
          panel1.Resize   += new EventHandler(PanelOnResize);
          panel1.Paint    += new PaintEventHandler(PanelOnPaint);

          Splitter split   = new Splitter();
          split.Parent     = this;
          split.Dock       = DockStyle.Right;

          Panel panel2     = new Panel();
          panel2.Parent    = this;
          panel2.Dock      = DockStyle.Right;
          panel2.BackColor = Color.Red;
          panel2.Resize   += new EventHandler(PanelOnResize);
          panel2.Paint    += new PaintEventHandler(PanelOnPaint);
     }
     void PanelOnResize(object obj, EventArgs ea)
     {
          ((Panel) obj).Invalidate();
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
     {
          Panel    panel = (Panel) obj;
          Graphics grfx  = pea.Graphics;

          grfx.DrawEllipse(Pens.Black, 0, 0, 
                           panel.Width - 1, panel.Height - 1);
     }
}
