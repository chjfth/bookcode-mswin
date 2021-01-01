//-----------------------------------------------
// SplitThreeFrames.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SplitThreeFrames: Form
{
     public static void Main()
     {
          Application.Run(new SplitThreeFrames());
     }
     public SplitThreeFrames()
     {
          Text = "Split Three Frames";

          Panel panel      = new Panel();
          panel.Parent     = this;
          panel.Dock       = DockStyle.Fill;

          Splitter split1  = new Splitter();
          split1.Parent    = this;
          split1.Dock      = DockStyle.Left;

          Panel panel1     = new Panel();
          panel1.Parent    = this;
          panel1.Dock      = DockStyle.Left;
          panel1.BackColor = Color.Lime;
          panel1.Resize   += new EventHandler(PanelOnResize);
          panel1.Paint    += new PaintEventHandler(PanelOnPaint);

          Panel panel2     = new Panel();
          panel2.Parent    = panel;
          panel2.Dock      = DockStyle.Fill;
          panel2.BackColor = Color.Blue;
          panel2.Resize   += new EventHandler(PanelOnResize);
          panel2.Paint    += new PaintEventHandler(PanelOnPaint);

          Splitter split2  = new Splitter();
          split2.Parent    = panel;
          split2.Dock      = DockStyle.Top;

          Panel panel3     = new Panel();
          panel3.Parent    = panel;
          panel3.Dock      = DockStyle.Top;
          panel3.BackColor = Color.Tan;
          panel3.Resize   += new EventHandler(PanelOnResize);
          panel3.Paint    += new PaintEventHandler(PanelOnPaint);

          panel1.Width  = ClientSize.Width  / 3;
          panel3.Height = ClientSize.Height / 3;
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
