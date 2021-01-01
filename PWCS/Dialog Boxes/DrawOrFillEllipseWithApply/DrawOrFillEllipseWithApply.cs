//---------------------------------------------------------
// DrawOrFillEllipseWithApply.cs (R) 2001 by Charles Petzold
//---------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class DrawOrFillEllipseWithApply: Form
{
     Color colorEllipse = Color.Red;
     bool  bFillEllipse = false;

     public static void Main()
     {
          Application.Run(new DrawOrFillEllipseWithApply());
     }
     public DrawOrFillEllipseWithApply()
     {
          Text = "Draw or Fill Ellipse with Apply";
          ResizeRedraw = true;

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Options");
          Menu.MenuItems[0].MenuItems.Add("&Color...", 
                                   new EventHandler(MenuColorOnClick));
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
          ColorFillDialogBoxWithApply dlg = 
                                   new ColorFillDialogBoxWithApply();

          dlg.ShowApply = true;
          dlg.Apply    += new EventHandler(ColorFillDialogOnApply);

          dlg.Color = colorEllipse;
          dlg.Fill  = bFillEllipse;

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               colorEllipse = dlg.Color;
               bFillEllipse = dlg.Fill;
               Invalidate();
          }
     }
     void ColorFillDialogOnApply(object obj, EventArgs ea)
     {
          ColorFillDialogBoxWithApply dlg = 
                                   (ColorFillDialogBoxWithApply) obj;

          colorEllipse = dlg.Color;
          bFillEllipse = dlg.Fill;
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics  grfx = pea.Graphics;
          Rectangle rect = new Rectangle(0, 0, ClientSize.Width - 1,
                                               ClientSize.Height - 1);
          if(bFillEllipse)
               grfx.FillEllipse(new SolidBrush(colorEllipse), rect);
          else
               grfx.DrawEllipse(new Pen(colorEllipse), rect);
     }
}
