//----------------------------------------------
// MetafileConvert.cs © 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;              // For Path class
using System.Windows.Forms;

class MetafileConvert: MetafileViewer
{
     public new static void Main()
     {
          Application.Run(new MetafileConvert());
     }
     public MetafileConvert()
     {
          Text = strProgName = "Metafile Convert";
     }
     protected override void MenuFileSaveAsOnClick(object obj, EventArgs ea)
     {
          SaveFileDialog dlg = new SaveFileDialog();

          if (strFileName != null && strFileName.Length > 0)
               dlg.InitialDirectory = Path.GetDirectoryName(strFileName);

          dlg.FileName = Path.GetFileNameWithoutExtension(strFileName);
          dlg.AddExtension = true;
          dlg.Filter = "Windows Bitmap (*.bmp)|*.bmp|" +
                       "Graphics Interchange Format (*.gif)|*.gif|" +
                       "JPEG File Interchange Format (*.jpg)|" +
                              "*.jpg;*.jpeg;*.jfif|" +
                       "Portable Network Graphics (*.png)|*.png|" +
                       "Tagged Image File Format (*.tif)|*.tif;*.tiff";

          if (dlg.ShowDialog() == DialogResult.OK)
          {
               Bitmap bm = MetafileToBitmap(mf);

               try
               {
                    bm.Save(dlg.FileName);
               }
               catch (Exception exc)
               {
                    MessageBox.Show(exc.Message, Text);
               }
          }
     }
     Bitmap MetafileToBitmap(Metafile mf)
     {
          Graphics grfx = CreateGraphics();
          int cx = (int) (grfx.DpiX * mf.Width  / mf.HorizontalResolution);
          int cy = (int) (grfx.DpiY * mf.Height / mf.VerticalResolution);
          Bitmap bm = new Bitmap(cx, cy, grfx);
          grfx.Dispose();

          grfx = Graphics.FromImage(bm);
          grfx.DrawImage(mf, 0, 0, cx, cy);
          grfx.Dispose();

          return bm;
     }
}
