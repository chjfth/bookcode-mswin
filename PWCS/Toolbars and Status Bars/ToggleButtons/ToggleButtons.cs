//--------------------------------------------
// ToggleButtons.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ToggleButtons: Form
{
     protected Panel   panel;
     protected ToolBar tbar;
     protected string  strText = "Toggle";
     protected Color   clrText = SystemColors.WindowText;
     FontStyle         fontstyle = FontStyle.Regular;

     public static void Main()
     {
          Application.Run(new ToggleButtons());
     }
     public ToggleButtons()
     {
          Text = "Toggle Buttons";

               // Create panel to fill interior.

          panel = new Panel();
          panel.Parent = this;
          panel.Dock = DockStyle.Fill;
          panel.BackColor = SystemColors.Window;
          panel.ForeColor = SystemColors.WindowText;
          panel.Resize += new EventHandler(PanelOnResize);
          panel.Paint += new PaintEventHandler(PanelOnPaint);

               // Create ImageList.

          Bitmap bm = new Bitmap(GetType(), 
                                 "ToggleButtons.FontStyleButtons.bmp");

          ImageList imglst = new ImageList();
          imglst.ImageSize = new Size(bm.Width / 4, bm.Height);
          imglst.Images.AddStrip(bm);
          imglst.TransparentColor = Color.White;

               // Create ToolBar.

          tbar = new ToolBar();
          tbar.ImageList = imglst;
          tbar.Parent = this;
          tbar.ShowToolTips = true;
          tbar.ButtonClick += 
                         new ToolBarButtonClickEventHandler(ToolBarOnClick);

               // Create ToolBarButtons.

          FontStyle[] afs = { FontStyle.Bold, FontStyle.Italic,
                              FontStyle.Underline, FontStyle.Strikeout };

          for (int i = 0; i < 4; i++)
          {
               ToolBarButton tbarbtn = new ToolBarButton();
               tbarbtn.ImageIndex = i;
               tbarbtn.Style = ToolBarButtonStyle.ToggleButton;
               tbarbtn.ToolTipText = afs[i].ToString();
               tbarbtn.Tag = afs[i];

               tbar.Buttons.Add(tbarbtn);
          }
     }
     void ToolBarOnClick(object obj, ToolBarButtonClickEventArgs tbbcea)
     {
          ToolBarButton tbarbtn = tbbcea.Button;

               // If the Tag isn't a FontStyle, don't do anything.

          if (tbarbtn.Tag == null || 
			  tbarbtn.Tag.GetType() != typeof(FontStyle))
               return;

               // Set or clear the bit in the fontstyle field.

          if (tbarbtn.Pushed)
               fontstyle |= (FontStyle) tbarbtn.Tag;
          else
               fontstyle &= ~(FontStyle) tbarbtn.Tag;

          panel.Invalidate();
     }
     void PanelOnResize(object obj, EventArgs ea)
     {
          Panel panel = (Panel) obj;
          panel.Invalidate();
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
     {
          Panel    panel = (Panel) obj;
          Graphics grfx  = pea.Graphics;
          Font     font  = new Font("Times New Roman", 72, fontstyle);
          SizeF    sizef = grfx.MeasureString(strText, font);

          grfx.DrawString(strText, font, new SolidBrush(clrText),
                          (panel.Width - sizef.Width) / 2,
                          (panel.Height - sizef.Height) / 2);
     }
}