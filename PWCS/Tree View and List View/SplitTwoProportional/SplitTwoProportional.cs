//---------------------------------------------------
// SplitTwoProportional.cs © 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SplitTwoProportional: Form
{
     Panel panel2;
     float fProportion = 0.5f;

     public static void Main()
     {
          Application.Run(new SplitTwoProportional());
     }
     public SplitTwoProportional()
     {
          Text = "Split Two Proportional";

          Panel panel1     = new Panel();
          panel1.Parent    = this;
          panel1.Dock      = DockStyle.Fill;
          panel1.BackColor = Color.Red;
          panel1.Resize   += new EventHandler(PanelOnResize);
          panel1.Paint    += new PaintEventHandler(PanelOnPaint);

          Splitter split   = new Splitter();
          split.Parent     = this;
          split.Dock       = DockStyle.Left;
          split.SplitterMoving += new SplitterEventHandler(SplitterOnMoving);

          panel2           = new Panel();
          panel2.Parent    = this;
          panel2.Dock      = DockStyle.Left;
          panel2.BackColor = Color.Lime;
          panel2.Resize   += new EventHandler(PanelOnResize);
          panel2.Paint    += new PaintEventHandler(PanelOnPaint);

          OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);
          panel2.Width = (int) (fProportion * ClientSize.Width);
     }
     void SplitterOnMoving(object obj, SplitterEventArgs sea)
     {
          fProportion = (float) sea.SplitX / ClientSize.Width;
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
