//---------------------------------------
// ClipView.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

class ClipView: Form
{
     string[] astrFormats = 
     { 
     DataFormats.Bitmap, DataFormats.CommaSeparatedValue, DataFormats.Dib,
     DataFormats.Dif, DataFormats.EnhancedMetafile, DataFormats.FileDrop, 
     DataFormats.Html, DataFormats.Locale, DataFormats.MetafilePict, 
     DataFormats.OemText, DataFormats.Palette, DataFormats.PenData, 
     DataFormats.Riff, DataFormats.Rtf, DataFormats.Serializable, 
     DataFormats.StringFormat, DataFormats.SymbolicLink, DataFormats.Text, 
     DataFormats.Tiff, DataFormats.UnicodeText, DataFormats.WaveAudio 
     };
     
     Panel         panelDisplay;
     RadioButton[] aradio;
     RadioButton   radioChecked;

     public static void Main()
     {
          Application.Run(new ClipView());
     }
     public ClipView()
     {
          Text = "Clipboard Viewer";

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

          Panel panel = new Panel();
          panel.Parent = this;
          panel.Dock = DockStyle.Left;
          panel.Width = 200;

               // Create radio buttons.
          
          aradio = new RadioButton[astrFormats.Length];
          EventHandler eh = new EventHandler(RadioButtonOnClick);

          for (int i = 0; i < astrFormats.Length; i++)
          {
               aradio[i] = new RadioButton();
               aradio[i].Parent = panel;
               aradio[i].Location = new Point(4, 12 * i);
               aradio[i].Size = new Size(300, 12);
               aradio[i].Click += eh;
               aradio[i].Tag = astrFormats[i];
          }
               // Set autoscale base size.

          AutoScaleBaseSize = new Size(4, 8);

               // Set time for 1 second.

          Timer timer = new Timer();
          timer.Interval = 1000;
          timer.Tick += new EventHandler(TimerOnTick);
          timer.Enabled = true;
     }
     void TimerOnTick(object obj, EventArgs ea)
     {
          IDataObject data = Clipboard.GetDataObject();

          for (int i = 0; i < astrFormats.Length; i++)
          {
               aradio[i].Text = astrFormats[i];
               aradio[i].Enabled = data.GetDataPresent(astrFormats[i]);

               if (aradio[i].Enabled)
               {
                    if (!data.GetDataPresent(astrFormats[i], false))
                         aradio[i].Text += "*";

                    object objClip = data.GetData(astrFormats[i]);

                    try
                    {
                         aradio[i].Text += " (" + objClip.GetType() + ")";
                    }
                    catch
                    {
                         aradio[i].Text += " (Exception on GetType!)";
                    }
               }
          }
          panelDisplay.Invalidate();
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

          if (radioChecked == null || !radioChecked.Enabled)
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