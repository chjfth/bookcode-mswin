//----------------------------------------
// ImageDrop.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

class ImageDrop: ImageClip
{
     bool bIsTarget;

     public new static void Main()
     {
		 Application.Run(new ImageDrop());
     }
     public ImageDrop()
     {
          Text = strProgName = "Image Drop";

          AllowDrop = true;
     }
     protected override void OnDragOver(DragEventArgs dea)
     {
          if (dea.Data.GetDataPresent(DataFormats.FileDrop) ||
              dea.Data.GetDataPresent(typeof(Metafile))     ||
              dea.Data.GetDataPresent(typeof(Bitmap)))
          {
               if ((dea.AllowedEffect & DragDropEffects.Move) != 0)
                    dea.Effect = DragDropEffects.Move;

               if (((dea.AllowedEffect & DragDropEffects.Copy) != 0) &&
                   ((dea.KeyState & 0x08) != 0))    // Ctrl key
                    dea.Effect = DragDropEffects.Copy;
          }
     }
     protected override void OnDragDrop(DragEventArgs dea)
     {
          if (dea.Data.GetDataPresent(DataFormats.FileDrop))
          {
               string[] astr = (string[]) 
                                   dea.Data.GetData(DataFormats.FileDrop);
               try
               {
                    image = Image.FromFile(astr[0]);
               }
               catch (Exception exc)
               {
                    MessageBox.Show(exc.Message, Text);
                    return;
               }
               strFileName = astr[0];
               Text = strProgName + " - " + Path.GetFileName(strFileName);
               Invalidate();
          }
          else 
          {
               if (dea.Data.GetDataPresent(typeof(Metafile)))
                    image = (Image) dea.Data.GetData(typeof(Metafile));

               else if (dea.Data.GetDataPresent(typeof(Bitmap)))
                    image = (Image) dea.Data.GetData(typeof(Bitmap));

               bIsTarget = true;
               strFileName = "DragAndDrop";
               Text = strProgName + " - " + strFileName;
               Invalidate();
          }
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (image != null)
          {
               bIsTarget = false;

               DragDropEffects dde = DoDragDrop(image, 
                              DragDropEffects.Copy | DragDropEffects.Move);

               if (dde == DragDropEffects.Move && !bIsTarget)
                    image = null;
          }
     }
}
