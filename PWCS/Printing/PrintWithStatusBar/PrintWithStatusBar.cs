//-------------------------------------------------
// PrintWithStatusBar.cs © 2001 by Charles Petzold
//-------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class PrintWithStatusBar: Form
{
     StatusBar      sbar;
     StatusBarPanel sbarpanel;

     const int iNumberPages = 3;
     int       iPageNumber;

     public static void Main()
     {
          Application.Run(new PrintWithStatusBar());
     }
     public PrintWithStatusBar()
     {
          Text = "Print with Status Bar";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&File");
          Menu.MenuItems[0].MenuItems.Add("&Print", 
                                   new EventHandler(MenuFilePrintOnClick));

          sbar = new StatusBar();
          sbar.Parent = this;
          sbar.ShowPanels = true;

          sbarpanel = new StatusBarPanel();
          sbarpanel.Text = "Ready";
          sbarpanel.Width = Width / 2;
          sbar.Panels.Add(sbarpanel);
     }
     void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
          PrintDocument prndoc   = new PrintDocument();
          prndoc.DocumentName    = Text;
          prndoc.PrintController = new StatusBarPrintController(sbarpanel);
          prndoc.PrintPage      += new PrintPageEventHandler(OnPrintPage);

          iPageNumber = 1;
          prndoc.Print();
     }
     void OnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics grfx  = ppea.Graphics;
          Font     font  = new Font("Times New Roman", 360);
          string   str   = iPageNumber.ToString();
          SizeF    sizef = grfx.MeasureString(str, font);

          grfx.DrawString(str, font, Brushes.Black, 
                    (grfx.VisibleClipBounds.Width - sizef.Width) / 2,
                    (grfx.VisibleClipBounds.Height - sizef.Height) / 2);

          System.Threading.Thread.Sleep(1000);    

          ppea.HasMorePages = iPageNumber < iNumberPages;
          iPageNumber += 1;
     }
}
