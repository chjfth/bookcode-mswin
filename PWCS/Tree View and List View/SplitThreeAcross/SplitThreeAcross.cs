//-----------------------------------------------
// SplitThreeAcross.cs (R) 2001 by Charles Petzold
//-----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SplitThreeAcross: Form
{
     public static void Main()
     {
          Application.Run(new SplitThreeAcross());
     }
     public SplitThreeAcross()
     {
          Text = "Split Three Across";

          Panel panel1     = new Panel();
          panel1.Parent    = this;
          panel1.Dock      = DockStyle.Fill;
          panel1.BackColor = Color.Cyan;
          panel1.Resize   += new EventHandler(PanelOnResize);
          panel1.Paint    += new PaintEventHandler(PanelOnPaint);

          Splitter split1  = new Splitter();
          split1.Parent    = this;
          split1.Dock      = DockStyle.Left;

          Panel panel2     = new Panel();
          panel2.Parent    = this;
          panel2.Dock      = DockStyle.Left;
          panel2.BackColor = Color.Lime;
          panel2.Resize   += new EventHandler(PanelOnResize);
          panel2.Paint    += new PaintEventHandler(PanelOnPaint);

          Splitter split2  = new Splitter();
          split2.Parent    = this;
          split2.Dock      = DockStyle.Right;

          Panel panel3     = new Panel();
          panel3.Parent    = this;
          panel3.Dock      = DockStyle.Right;
          panel3.BackColor = Color.Red;
          panel3.Resize   += new EventHandler(PanelOnResize);
          panel3.Paint    += new PaintEventHandler(PanelOnPaint);

          panel1.Width = 
          panel2.Width = 
          panel3.Width = ClientSize.Width / 3;
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
