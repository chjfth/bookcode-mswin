//--------------------------------------------------------
// BetterFontAndColorDialogs.cs © 2001 by Charles Petzold
//--------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterFontAndColorDialogs:Form
{
     protected ColorDialog clrdlg = new ColorDialog();

     public static void Main()
     {
          Application.Run(new BetterFontAndColorDialogs());
     }
     public BetterFontAndColorDialogs()
     {
          Text = "Better Font and Color Dialogs";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Format");
          Menu.MenuItems[0].MenuItems.Add("&Font...",
                                        new EventHandler(MenuFontOnClick));
          Menu.MenuItems[0].MenuItems.Add("&Background Color...",
                                        new EventHandler(MenuColorOnClick));
     }
     void MenuFontOnClick(object obj, EventArgs ea)
     {
          FontDialog fontdlg = new FontDialog();

          fontdlg.Font  = Font;
          fontdlg.Color = ForeColor;
          fontdlg.ShowColor = true;
          fontdlg.ShowApply = true;
          fontdlg.Apply += new EventHandler(FontDialogOnApply);

          if(fontdlg.ShowDialog() == DialogResult.OK)
          {
               Font = fontdlg.Font;
               ForeColor = fontdlg.Color;
               Invalidate();
          }
     }
     void MenuColorOnClick(object obj, EventArgs ea)
     {
          clrdlg.Color = BackColor;

          if (clrdlg.ShowDialog() == DialogResult.OK)
               BackColor = clrdlg.Color;
     }
      void FontDialogOnApply(object obj, EventArgs ea)
     {
          FontDialog fontdlg = (FontDialog) obj;

          Font = fontdlg.Font;
          ForeColor = fontdlg.Color;
          Invalidate();
     }
    protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          grfx.DrawString("Hello common dialog boxes!", Font, 
                          new SolidBrush(ForeColor), 0, 0);
     }
}