//-------------------------------------------
// ImageFromWeb.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.IO;
using System.Net;
using System.Windows.Forms;

class ImageFromWeb: PrintableForm
{
     Image image;

     public new static void Main()
     {
          Application.Run(new ImageFromWeb());
     }
     public ImageFromWeb()
     {
          Text = "Image From Web";

          string strUrl =
               "http://images.jsc.nasa.gov/images/pao/AS11/10075267.jpg";

          WebRequest   webreq = WebRequest.Create(strUrl);
          WebResponse  webres = webreq.GetResponse();
          Stream       stream = webres.GetResponseStream();

          image = Image.FromStream(stream);
          stream.Close();
     }
     protected override void DoPage(Graphics grfx, Color clr, int cx, int cy)
     {
          grfx.DrawImage(image, 0, 0);
     }
}
