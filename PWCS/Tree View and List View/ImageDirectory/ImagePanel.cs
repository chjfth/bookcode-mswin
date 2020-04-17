//-----------------------------------------
// ImagePanel.cs © 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

class ImagePanel: Panel
{
     const int cxButton = 100, cyButton = 100;         // Image button size

     Button    btnClicked;
     ToolTip   tooltip = new ToolTip();
     Timer     timer = new Timer();

          // Fields for Timer Tick event

     string[]  astrFileNames;
     int       i, x, y;
                                                       // Public event
     public event EventHandler ImageClicked;
                                                       // Constructor
     public ImagePanel()
     {
          AutoScroll = true;

          timer.Interval = 1;
          timer.Tick += new EventHandler(TimerOnTick);
     }
                                                       // Public properties
     public Control ClickedControl
     {
          get { return btnClicked; }
     }
     public Image ClickedImage
     {
          get 
          { 
               try
               {
                    return Image.FromFile((string) btnClicked.Tag);
               }
               catch
               {
                    return null;
               }
          }
     }
                                                       // Public method     
     public void ShowImages(string strDirectory)
     {
          Controls.Clear();
          tooltip.RemoveAll();

          try
          {
               astrFileNames = Directory.GetFiles(strDirectory);
          }
          catch
          {
               return;
          }

          i = x = y = 0;

          timer.Start();
     }
                                                       // Event handlers
     void TimerOnTick(object obj, EventArgs ea)
     {
          Image image;

          if (i == astrFileNames.Length)
          {
               timer.Stop();
               return;
          }
          try
          {
               image = Image.FromFile(astrFileNames[i]);
          }
          catch
          {
               i++;
               return;
          }
          int cxImage = image.Width;
          int cyImage = image.Height;

               // Convert image to small size for button.

          SizeF sizef = new SizeF(cxImage / image.HorizontalResolution,
                                  cyImage / image.VerticalResolution);

          float fScale = Math.Min(cxButton / sizef.Width, 
                                  cyButton / sizef.Height);
          sizef.Width *= fScale;
          sizef.Height *= fScale;
          Size size = Size.Ceiling(sizef);
          Bitmap bitmap = new Bitmap(image, size);
          image.Dispose();

               // Create button and add to panel.

          Button btn   = new Button();
          btn.Image    = bitmap;
          btn.Location = new Point(x, y) + (Size) AutoScrollPosition;
          btn.Size     = new Size(cxButton, cyButton);
          btn.Tag      = astrFileNames[i];
          btn.Click   += new EventHandler(ButtonOnClick);
          Controls.Add(btn);

               // Give button a ToolTip.

          tooltip.SetToolTip(btn, String.Format("{0}\n{1}x{2}",
                                        Path.GetFileName(astrFileNames[i]), 
                                        cxImage, cyImage));

               // Adjust i, x, and y for next image.

          AdjustXY(ref x, ref y);
          i++;
     }
     void ButtonOnClick(object obj, EventArgs ea)
     {
          btnClicked = (Button) obj;

          if (ImageClicked != null)
               ImageClicked(this, EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          AutoScrollPosition = Point.Empty;
          int x = 0, y = 0;

          foreach (Control cntl in Controls)
          {
               cntl.Location = new Point(x, y) + (Size) AutoScrollPosition;
               AdjustXY(ref x, ref y);
          }
     }
     void AdjustXY(ref int x, ref int y)
     {
          y += cyButton;

          if (y + cyButton > Height - 
                              SystemInformation.HorizontalScrollBarHeight)
          {
               y = 0;
               x += cxButton;
          }
     }
}
