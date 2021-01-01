//---------------------------------------------------
// NotepadCloneWithEdit.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class NotepadCloneWithEdit: NotepadCloneWithFile
{
     MenuItem miEditUndo, miEditCut, miEditCopy, miEditPaste, miEditDelete;
     string   strFind = "", strReplace = "";
     bool     bMatchCase = false, bFindDown = true;

     public new static void Main()
     {
          Application.Run(new NotepadCloneWithEdit());
     }
     public NotepadCloneWithEdit()
     {
          strProgName = "Notepad Clone with Edit";
          MakeCaption();
          
               // Edit menu

          MenuItem mi = new MenuItem("&Edit");
          mi.Popup += new EventHandler(MenuEditOnPopup);
          Menu.MenuItems.Add(mi);
          int index = Menu.MenuItems.Count - 1;

               // Edit Undo menu item

          miEditUndo = new MenuItem("&Undo");
          miEditUndo.Click += new EventHandler(MenuEditUndoOnClick);
          miEditUndo.Shortcut = Shortcut.CtrlZ;
          Menu.MenuItems[index].MenuItems.Add(miEditUndo);
          Menu.MenuItems[index].MenuItems.Add("-");

               // Edit Cut menu item

          miEditCut = new MenuItem("Cu&t");
          miEditCut.Click += new EventHandler(MenuEditCutOnClick);
          miEditCut.Shortcut = Shortcut.CtrlX;
          Menu.MenuItems[index].MenuItems.Add(miEditCut);

               // Edit Copy menu item

          miEditCopy = new MenuItem("&Copy");
          miEditCopy.Click += new EventHandler(MenuEditCopyOnClick);
          miEditCopy.Shortcut = Shortcut.CtrlC;
          Menu.MenuItems[index].MenuItems.Add(miEditCopy);

               // Edit Paste menu item

          miEditPaste = new MenuItem("&Paste");
          miEditPaste.Click += new EventHandler(MenuEditPasteOnClick);
          miEditPaste.Shortcut = Shortcut.CtrlV;
          Menu.MenuItems[index].MenuItems.Add(miEditPaste);

               // Edit Delete menu item

          miEditDelete = new MenuItem("De&lete");
          miEditDelete.Click += new EventHandler(MenuEditDeleteOnClick);
          miEditDelete.Shortcut = Shortcut.Del;
          Menu.MenuItems[index].MenuItems.Add(miEditDelete);
          Menu.MenuItems[index].MenuItems.Add("-");

               // Edit Find menu item

          mi = new MenuItem("&Find...");
          mi.Click += new EventHandler(MenuEditFindOnClick);
          mi.Shortcut = Shortcut.CtrlF;
          Menu.MenuItems[index].MenuItems.Add(mi);

               // Edit Find Next menu item

          mi = new MenuItem("Find &Next");
          mi.Click += new EventHandler(MenuEditFindNextOnClick);
          mi.Shortcut = Shortcut.F3;
          Menu.MenuItems[index].MenuItems.Add(mi);

               // Edit Replace menu item

          mi = new MenuItem("&Replace...");
          mi.Click += new EventHandler(MenuEditReplaceOnClick);
          mi.Shortcut = Shortcut.CtrlH;
          Menu.MenuItems[index].MenuItems.Add(mi);
          Menu.MenuItems[index].MenuItems.Add("-");

               // Edit Select All menu item

          mi = new MenuItem("Select &All");
          mi.Click += new EventHandler(MenuEditSelectAllOnClick);
          mi.Shortcut = Shortcut.CtrlA;
          Menu.MenuItems[index].MenuItems.Add(mi);

               // Edit Time/Date menu item

          mi = new MenuItem("Time/&Date");
          mi.Click += new EventHandler(MenuEditTimeDateOnClick);
          mi.Shortcut = Shortcut.F5;
          Menu.MenuItems[index].MenuItems.Add(mi);
     }
     void MenuEditOnPopup(object obj, EventArgs ea)
     {
          miEditUndo.Enabled = txtbox.CanUndo;

          miEditCut.Enabled = 
          miEditCopy.Enabled = 
          miEditDelete.Enabled  = (txtbox.SelectionLength > 0);

          miEditPaste.Enabled = 
               Clipboard.GetDataObject().GetDataPresent(typeof(string));
     }
     void MenuEditUndoOnClick(object obj, EventArgs ea)
     {
          txtbox.Undo();
          txtbox.ClearUndo();
     }
     void MenuEditCutOnClick(object obj, EventArgs ea)
     {
          txtbox.Cut();
     }
     void MenuEditCopyOnClick(object obj, EventArgs ea)
     {
          txtbox.Copy();
     }
     void MenuEditPasteOnClick(object obj, EventArgs ea)
     {
          txtbox.Paste();
     }
     void MenuEditDeleteOnClick(object obj, EventArgs ea)
     {
          txtbox.Clear();
     }
     void MenuEditFindOnClick(object obj, EventArgs ea)
     {
          if (OwnedForms.Length > 0)
               return;

          txtbox.HideSelection = false;

          FindDialog dlg = new FindDialog();

          dlg.Owner     = this;
          dlg.FindText  = strFind;
          dlg.MatchCase = bMatchCase;
          dlg.FindDown  = bFindDown;
          dlg.FindNext += new EventHandler(FindDialogOnFindNext);
          dlg.CloseDlg += new EventHandler(FindReplaceDialogOnCloseDlg);
          dlg.Show();
     }
     void MenuEditFindNextOnClick(object obj, EventArgs ea)
     {
          if (strFind.Length == 0)
          {
               if (OwnedForms.Length > 0)
                    return;

               MenuEditFindOnClick(obj, ea);
          }
          else
               FindNext();
     }
     void MenuEditReplaceOnClick(object obj, EventArgs ea)
     {
          if (OwnedForms.Length > 0)
               return;

          txtbox.HideSelection = false;

          ReplaceDialog dlg = new ReplaceDialog();

          dlg.Owner       = this;
          dlg.FindText    = strFind;
          dlg.ReplaceText = strReplace;
          dlg.MatchCase   = bMatchCase;
          dlg.FindDown    = bFindDown;
          dlg.FindNext   += new EventHandler(FindDialogOnFindNext);
          dlg.Replace    += new EventHandler(ReplaceDialogOnReplace);
          dlg.ReplaceAll += new EventHandler(ReplaceDialogOnReplaceAll);
          dlg.CloseDlg   += new EventHandler(FindReplaceDialogOnCloseDlg);
          dlg.Show();
     }
     void MenuEditSelectAllOnClick(object obj, EventArgs ea)
     {
          txtbox.SelectAll();
     }
     void MenuEditTimeDateOnClick(object obj, EventArgs ea)
     {
          DateTime dt = DateTime.Now;
          txtbox.SelectedText = dt.ToString("t") + " " + dt.ToString("d");
     }
     void FindDialogOnFindNext(object obj, EventArgs ea)
     {
          FindReplaceDialog dlg = (FindReplaceDialog) obj;

          strFind    = dlg.FindText;
          bMatchCase = dlg.MatchCase;
          bFindDown  = dlg.FindDown;

          FindNext();
     }
     bool FindNext()
     {
          if (bFindDown)
          {
               int iStart = txtbox.SelectionStart + txtbox.SelectionLength;

               while (iStart + strFind.Length <= txtbox.TextLength)
               {
                    if (string.Compare(strFind, 0, txtbox.Text, iStart, 
                                       strFind.Length, !bMatchCase) == 0)
                    {
                         txtbox.SelectionStart = iStart;
                         txtbox.SelectionLength = strFind.Length;
                         return true;
                    }
                    iStart++;
               }
          }
          else
          {
               int iStart = txtbox.SelectionStart - strFind.Length;

               while (iStart >= 0)
               {
                    if (string.Compare(strFind, 0, txtbox.Text, iStart, 
                                       strFind.Length, !bMatchCase) == 0)
                    {
                         txtbox.SelectionStart = iStart;
                         txtbox.SelectionLength = strFind.Length;
                         return true;
                    }
                    iStart--;
               }
          }
          MessageBox.Show("Cannot find \"" + strFind + "\"", strProgName,
                          MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
          return false;
     }
     void ReplaceDialogOnReplace(object obj, EventArgs ea)
     {
          FindReplaceDialog dlg = (FindReplaceDialog) obj;

          strFind    = dlg.FindText;
          strReplace = dlg.ReplaceText;
          bMatchCase = dlg.MatchCase;

          if (string.Compare(strFind, txtbox.SelectedText, !bMatchCase) == 0)
          {
               txtbox.SelectedText = strReplace;
          }
          FindNext();
     }
     void ReplaceDialogOnReplaceAll(object obj, EventArgs ea)
     {
          FindReplaceDialog dlg = (FindReplaceDialog) obj;

          string str = txtbox.Text;
          strFind    = dlg.FindText;
          strReplace = dlg.ReplaceText;
          bMatchCase = dlg.MatchCase;

          if (bMatchCase)
          {
               str = str.Replace(strFind, strReplace);
          }
          else
          {
               for (int i = 0; i < str.Length - strFind.Length; )
               {
                    if (String.Compare(str, i, strFind, 0, 
                                             strFind.Length, true) == 0)
                    {
                         str = str.Remove(i, strFind.Length);
                         str = str.Insert(i, strReplace);
                         i += strReplace.Length;
                    }
                    else
                    {
                         i += 1;
                    }
               }
          }
          if (str != txtbox.Text)
          {
               txtbox.Text = str;
               txtbox.SelectionStart = 0;
               txtbox.SelectionLength = 0;
               txtbox.Modified = true;
          }
     }
     void FindReplaceDialogOnCloseDlg(object obj, EventArgs ea)
     {
          txtbox.HideSelection = true;
     }
}          
