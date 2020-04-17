//-------------------------------------------
// EnumMetafile.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;

class EnumMetafile: Form
{
     Metafile     mf;
     Panel        panel;
     TextBox      txtbox;
     string       strCaption;
     StringWriter strwrite;

     public static void Main()
     {
          Application.Run(new EnumMetafile());
     }
     public EnumMetafile()
     {
          Text = strCaption = "Enumerate Metafile";

               // Create the text box for displaying records.

          txtbox = new TextBox();
          txtbox.Parent = this;
          txtbox.Dock = DockStyle.Fill;
          txtbox.Multiline = true;
          txtbox.WordWrap = false;
          txtbox.ReadOnly = true;
          txtbox.TabStop = false;
          txtbox.ScrollBars = ScrollBars.Vertical;

               // Create the splitter between the panel and the text box.

          Splitter splitter = new Splitter();
          splitter.Parent = this;
          splitter.Dock = DockStyle.Left; // Right;

               // Create the panel for displaying the metafile.

          panel = new Panel();
          panel.Parent = this;
          panel.Dock = DockStyle.Left;
          panel.Paint += new PaintEventHandler(PanelOnPaint);

               // Create the menu.

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Open!", new EventHandler(MenuOpenOnClick));
     }
     void MenuOpenOnClick(object obj, EventArgs ea)
     {
          OpenFileDialog dlg = new OpenFileDialog();

          dlg.Filter = "All Metafiles|*.wmf;*.emf|" +
                       "Windows Metafile (*.wmf)|*.wmf|" +
                       "Enhanced Metafile (*.emf)|*.emf";

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               try
               {
                    mf = new Metafile(dlg.FileName);
               }
               catch (Exception exc)
               {
                    MessageBox.Show(exc.Message, strCaption);
                    return;
               }
               Text = strCaption + " - " + Path.GetFileName(dlg.FileName);
               panel.Invalidate();

                    // Enumerate the metafile for the text box.

               strwrite = new StringWriter();
               Graphics grfx = CreateGraphics();

               grfx.EnumerateMetafile(mf, new Point(0, 0), 
                    new Graphics.EnumerateMetafileProc(EnumMetafileProc));

               grfx.Dispose();
               txtbox.Text = strwrite.ToString();
               txtbox.SelectionLength = 0;
          }
     }
     bool EnumMetafileProc(EmfPlusRecordType eprt, int iFlags,
                           int iDataSize, IntPtr ipData, 
                           PlayRecordCallback prc)
     {
          strwrite.Write("{0} ({1}, {2})", eprt, iFlags, iDataSize);

          if (iDataSize > 0)
          {
               byte[] abyData = new Byte[iDataSize];
               Marshal.Copy(ipData, abyData, 0, iDataSize);

               foreach (byte by in abyData)
                    strwrite.Write(" {0:X2}", by);
          }
          strwrite.WriteLine();
          return true;
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
     {
          Panel    panel = (Panel) obj;
          Graphics grfx  = pea.Graphics;

          if (mf != null)
               grfx.DrawImage(mf, 0, 0);
     }
}