//------------------------------------------
// ClipViewAll.cs © 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

class ClipViewAll: Form
{
     Panel       panelDisplay, panelButtons;
     RadioButton radioChecked;
     string[]    astrFormatsSave = new string[0];

     public static void Main()
     {
          Application.Run(new ClipViewAll());
     }
     public ClipViewAll()
     {
          Text = "Clipboard Viewer (All Formats)";

               // Create variable-width panel for clipboard display.

          panelDisplay = new Panel();
          panelDisplay.Parent = this;
          panelDisplay.Dock = DockStyle.Fill;
          panelDisplay.Paint += new PaintEventHandler(PanelOnPaint);
          panelDisplay.BorderStyle = BorderStyle.Fixed3D;

               // Create splitter.

          Splitter split = new Splitter();
          split.Parent = this;
          split.Dock = DockStyle.Left;

               // Create panel for radio buttons.

          panelButtons = new Panel();
          panelButtons.Parent = this;
          panelButtons.Dock = DockStyle.Left;
          panelButtons.AutoScroll = true;
          panelButtons.Width = Width / 2;

               // Set time for 1 second.

          Timer timer = new Timer();
          timer.Interval = 1000;
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Enabled = true;
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          string[] astrFormats = data.GetFormats();
          bool bUpdate = false;

               // Determine whether clipboard formats have changed.

          if (astrFormats.Length != astrFormatsSave.Length)
               bUpdate = true;
          else
          {
               for (int i = 0; i < astrFormats.Length; i++)
                    if (astrFormats[i] != astrFormatsSave[i])
                    {
                         bUpdate = true;
                         break;
                    }
          }
               // Invalidate display regardless.

          panelDisplay.Invalidate();

               // Don't update buttons if formats haven't changed.

          if (!bUpdate)
               return;

               // Formats have changed, so re-create radio buttons.

          astrFormatsSave = astrFormats;
          panelButtons.Controls.Clear();
          Graphics grfx = CreateGraphics();
          EventHandler eh = new EventHandler(RadioButtonOnClick);
          int cxText = AutoScaleBaseSize.Width;
          int cyText = AutoScaleBaseSize.Height;

          for (int i = 0; i < astrFormats.Length; i++)
          {
               RadioButton radio = new RadioButton();
               radio.Parent = panelButtons;
               radio.Text = astrFormats[i];

               if (!data.GetDataPresent(astrFormats[i], false))
                    radio.Text += "*";

               try
               {
                    object objClip = data.GetData(astrFormats[i]);
                    radio.Text += " (" + objClip.GetType() + ")";
               }
               catch
               {
                    radio.Text += " (Exception on GetData or GetType!)";
               }
               radio.Tag = astrFormats[i];
               radio.Location = new Point(cxText, i * 3 * cyText / 2);
               radio.Size = new Size((radio.Text.Length + 20) * cxText, 
                                     3 * cyText / 2);
               radio.Click += eh;
          }
          grfx.Dispose();
          radioChecked = null;
     }
     void RadioButtonOnClick(object obj, EventArgs ea)
     {
          radioChecked = (RadioButton) obj;
          panelDisplay.Invalidate();
     }
     void PanelOnPaint(object obj, PaintEventArgs pea)
     {
          Panel    panel = (Panel) obj;
          Graphics grfx  = pea.Graphics;
          Brush    brush = new SolidBrush(panel.ForeColor);

          if (radioChecked == null)
               return;

          IDataObject data = Clipboard.GetDataObject();
          object objClip = data.GetData((string) radioChecked.Tag);

          if (objClip == null)
               return;

          else if (objClip.GetType() == typeof(string))
          {
               grfx.DrawString((string)objClip, Font, brush, 
                               panel.ClientRectangle);
          }
          else if (objClip.GetType() == typeof(string[]))   // FileDrop
          {
               string str = string.Join("\r\n", (string[]) objClip);

               grfx.DrawString(str, Font, brush, panel.ClientRectangle);
          }
          else if (objClip.GetType() == typeof(Bitmap) ||
                   objClip.GetType() == typeof(Metafile) ||
                   objClip.GetType() == typeof(Image))
          {
               grfx.DrawImage((Image)objClip, 0, 0);
          }
          else if (objClip.GetType() == typeof(MemoryStream))
          {
               Stream stream = (Stream) objClip;
               byte[] abyBuffer = new byte[16];
               long   lAddress = 0;
               int    iCount;
               Font   font = new Font(FontFamily.GenericMonospace, 
                                      Font.SizeInPoints);
               float  y = 0;

               while ((iCount = stream.Read(abyBuffer, 0, 16)) > 0)
               {
                    string str = HexDump.ComposeLine(lAddress, abyBuffer, 
                                                               iCount);
                    grfx.DrawString(str, font, brush, 0, y);
                    lAddress += 16;
                    y += font.GetHeight(grfx);

                    if (y > panel.Bottom)
                         break;
               }
          }
     }
}