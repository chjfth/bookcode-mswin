//---------------------------------------
// HeadDump.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class HeadDump: Form
{
     const string strProgName  = "Head Dump";
     string       strFileName = "";

     public static void Main()
     {
          Application.Run(new HeadDump());
     }
     public HeadDump()
     {
          Text = strProgName;
          Font = new Font(FontFamily.GenericMonospace, 
                          Font.SizeInPoints);

          Menu = new MainMenu();
          Menu.MenuItems.Add("&File");
          Menu.MenuItems[0].MenuItems.Add("&Open...", 
                              new EventHandler(MenuFileOpenOnClick));
          Menu.MenuItems.Add("F&ormat");
          Menu.MenuItems[1].MenuItems.Add("&Font...",
                              new EventHandler(MenuFormatFontOnClick));
     }
     void MenuFileOpenOnClick(object obj, EventArgs ea)
     {
          OpenFileDialog dlg = new OpenFileDialog();

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               strFileName = dlg.FileName;
               Text = strProgName + " - " + Path.GetFileName(strFileName);
               Invalidate();
          }
     }
     void MenuFormatFontOnClick(object obj, EventArgs ea)
     {
          FontDialog dlg = new FontDialog();

          dlg.Font = Font;
          dlg.FixedPitchOnly = true;

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               Font = dlg.Font;
               Invalidate();
          }
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics   grfx  = pea.Graphics;
          Brush      brush = new SolidBrush(ForeColor);
          FileStream fs;

          try
          {
               fs = new FileStream(strFileName, FileMode.Open,
                                   FileAccess.Read, FileShare.Read);
          }
          catch
          {
               return;
          }

          for (int iLine = 0; iLine <= ClientSize.Height / Font.Height; 
                              iLine++)
          {
               byte[] abyBuffer = new byte[16];
               int    iCount    = fs.Read(abyBuffer, 0, 16);
               string str       = HexDump.ComposeLine(16 * iLine, 
                                                      abyBuffer, iCount);

               grfx.DrawString(str, Font, brush, 0, iLine * Font.Height);
          }
          fs.Close();
     }
 }