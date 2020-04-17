//-------------------------------------------
// NotepadClone.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class NotepadClone: NotepadCloneWithPrinting
{
     public new static void Main()
     {
               // This needs to be done for drag-and-drop to work. 

          System.Threading.Thread.CurrentThread.ApartmentState = 
                                        System.Threading.ApartmentState.STA;

          Application.Run(new NotepadClone());
     }
     public NotepadClone()
     {
          strProgName = "NotepadClone";
          MakeCaption();

          txtbox.AllowDrop = true;
          txtbox.DragOver += new DragEventHandler(TextBoxOnDragOver);
          txtbox.DragDrop += new DragEventHandler(TextBoxOnDragDrop);
     }
     void TextBoxOnDragOver(object obj, DragEventArgs dea)
     {
          if (dea.Data.GetDataPresent(DataFormats.FileDrop) ||
              dea.Data.GetDataPresent(DataFormats.StringFormat))
          {
               if ((dea.AllowedEffect & DragDropEffects.Move) != 0)
                    dea.Effect = DragDropEffects.Move;

               if (((dea.AllowedEffect & DragDropEffects.Copy) != 0) &&
                   ((dea.KeyState & 0x08) != 0))    // Ctrl key
                    dea.Effect = DragDropEffects.Copy;
          }
     }
     void TextBoxOnDragDrop(object obj, DragEventArgs dea)
     {
          if (dea.Data.GetDataPresent(DataFormats.FileDrop))
          {
               if (!OkToTrash())
                    return;

               string[] astr = (string[]) 
                                   dea.Data.GetData(DataFormats.FileDrop);

               LoadFile(astr[0]);     // In NotepadCloneWithFile.cs
          }
          else if (dea.Data.GetDataPresent(DataFormats.StringFormat))
          {
               txtbox.SelectedText = 
                         (string) dea.Data.GetData(DataFormats.StringFormat);
          }
     }
}
