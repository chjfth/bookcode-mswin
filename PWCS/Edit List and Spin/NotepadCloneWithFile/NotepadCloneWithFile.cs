//---------------------------------------------------
// NotepadCloneWithFile.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using Microsoft.Win32;             // For registry classes
using System;
using System.ComponentModel;       // For CancelEventArgs class
using System.Drawing;
using System.IO;
using System.Text;                 // For Encoding class
using System.Windows.Forms;

class NotepadCloneWithFile: NotepadCloneWithRegistry
{
                                                            // Fields
     protected string strFileName;
     const     string strEncoding = "Encoding";        // For registry
     const     string strFilter = 
                         "Text Documents(*.txt)|*.txt|All Files(*.*)|*.*";
     MenuItem         miEncoding;
     MenuItemEncoding mieChecked;
                                                            // Entry point
     public new static void Main()
     {
          Application.Run(new NotepadCloneWithFile());
     }
                                                            // Constructor
     public NotepadCloneWithFile()
     {
          strProgName = "Notepad Clone with File";
          MakeCaption();
          Menu = new MainMenu();

               // File menu

          MenuItem mi = new MenuItem("&File");
          Menu.MenuItems.Add(mi);
          int index = Menu.MenuItems.Count - 1;

               // File New

          mi = new MenuItem("&New");
          mi.Click += new EventHandler(MenuFileNewOnClick);
          mi.Shortcut = Shortcut.CtrlN;
          Menu.MenuItems[index].MenuItems.Add(mi);

               // File Open

          MenuItem miFileOpen = new MenuItem("&Open...");
          miFileOpen.Click += new EventHandler(MenuFileOpenOnClick);
          miFileOpen.Shortcut = Shortcut.CtrlO;
          Menu.MenuItems[index].MenuItems.Add(miFileOpen);

               // File Save

          MenuItem miFileSave  = new MenuItem("&Save");
          miFileSave.Click += new EventHandler(MenuFileSaveOnClick);
          miFileSave.Shortcut = Shortcut.CtrlS;
          Menu.MenuItems[index].MenuItems.Add(miFileSave);

               // File Save As

          mi = new MenuItem("Save &As...");
          mi.Click += new EventHandler(MenuFileSaveAsOnClick);
          Menu.MenuItems[index].MenuItems.Add(mi);

               // File Encoding

          miEncoding = new MenuItem("&Encoding");
          Menu.MenuItems[index].MenuItems.Add(miEncoding);
          Menu.MenuItems[index].MenuItems.Add("-");

               // File Encoding submenu

          EventHandler eh = new EventHandler(MenuFileEncodingOnClick);

          string[] astrEncodings = { "&ASCII", "&Unicode", 
                                     "&Big-Endian Unicode",
                                     "UTF-&7", "&UTF-&8" };

          Encoding[] aenc = { Encoding.ASCII, Encoding.Unicode, 
                              Encoding.BigEndianUnicode, 
                              Encoding.UTF7, Encoding.UTF8 };

          for (int i = 0; i < astrEncodings.Length; i++)
          {
               MenuItemEncoding mie = new MenuItemEncoding();
               mie.Text = astrEncodings[i];
               mie.Encoding = aenc[i];
               mie.RadioCheck = true;
               mie.Click += eh;

               miEncoding.MenuItems.Add(mie);
          }
          mieChecked = (MenuItemEncoding) miEncoding.MenuItems[4];  // UTF-8
          mieChecked.Checked = true;

               // File Page Setup

          mi = new MenuItem("Page Set&up...");
          mi.Click += new EventHandler(MenuFileSetupOnClick);
          Menu.MenuItems[index].MenuItems.Add(mi);

               // File Print Preview

          mi = new MenuItem("Print Pre&view...");
          mi.Click += new EventHandler(MenuFilePreviewOnClick);
          Menu.MenuItems[index].MenuItems.Add(mi);

               // File Print

          mi = new MenuItem("&Print...");
          mi.Click += new EventHandler(MenuFilePrintOnClick);
          mi.Shortcut = Shortcut.CtrlP;
          Menu.MenuItems[index].MenuItems.Add(mi);
          Menu.MenuItems[index].MenuItems.Add("-");

               // File Exit

          mi = new MenuItem("E&xit");
          mi.Click += new EventHandler(MenuFileExitOnClick);
          Menu.MenuItems[index].MenuItems.Add(mi);

               // Set system event.

          SystemEvents.SessionEnding += 
                              new SessionEndingEventHandler(OnSessionEnding);
     }
                                                       // Event overrides
     protected override void OnLoad(EventArgs ea)
     {
          base.OnLoad(ea);     

               // Deal with the command-line argument.

          string[] astrArgs = Environment.GetCommandLineArgs();

          if (astrArgs.Length > 1)      // First argument is program name!
          {
               if (File.Exists(astrArgs[1]))
               {
                    LoadFile(astrArgs[1]);
               }
               else
               {
                    DialogResult dr = 
                         MessageBox.Show("Cannot find the " +
                                         Path.GetFileName(astrArgs[1]) + 
                                         " file.\r\n\r\n" +
                                         "Do you want to create a new file?",
                                         strProgName,
                                         MessageBoxButtons.YesNoCancel,
                                         MessageBoxIcon.Question);
                    switch(dr)
                    {
                    case DialogResult.Yes:   // Create and close file.
                         File.Create(strFileName = astrArgs[1]).Close();
                         MakeCaption();
                         break;

                    case DialogResult.No:
                         break;

                    case DialogResult.Cancel:
                         Close();
                         break;
                    }
               }
          }
     }
     protected override void OnClosing(CancelEventArgs cea)
     {
          base.OnClosing(cea);

          cea.Cancel = !OkToTrash();
     }
                                                            // Event handlers
     void OnSessionEnding(object obj, SessionEndingEventArgs seea)
     {
          seea.Cancel = !OkToTrash();          
     }
                                                            // Menu items
     void MenuFileNewOnClick(object obj, EventArgs ea)
     {
          if (!OkToTrash())
               return;

          txtbox.Clear();
          txtbox.ClearUndo();
          txtbox.Modified = false;

          strFileName = null;
          MakeCaption();
     }
     void MenuFileOpenOnClick(object obj, EventArgs ea)
     {
          if (!OkToTrash())
               return;

          OpenFileDialog ofd = new OpenFileDialog();
          ofd.Filter = strFilter;
          ofd.FileName = "*.txt";

          if (ofd.ShowDialog() == DialogResult.OK)
               LoadFile(ofd.FileName);
     }
     void MenuFileEncodingOnClick(object obj, EventArgs ea)
     {
          mieChecked.Checked = false;
          mieChecked = (MenuItemEncoding) obj;
          mieChecked.Checked = true;
     }
     void MenuFileSaveOnClick(object obj, EventArgs ea)
     {
          if (strFileName == null || strFileName.Length == 0)
               SaveFileDlg();
          else
               SaveFile();
     }
     void MenuFileSaveAsOnClick(object obj, EventArgs ea)
     {
          SaveFileDlg();
     }
     protected virtual void MenuFileSetupOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Page Setup not yet implemented!", strProgName);
     }
     protected virtual void MenuFilePreviewOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Print Preview not yet implemented!", strProgName);
     }
     protected virtual void MenuFilePrintOnClick(object obj, EventArgs ea)
     {
          MessageBox.Show("Print not yet implemented!", strProgName);
     }
     void MenuFileExitOnClick(object obj, EventArgs ea)
     {
          if (OkToTrash())
               Application.Exit();
     }
                                                       // Method overrides
     protected override void LoadRegistryInfo(RegistryKey regkey)
     {
          base.LoadRegistryInfo(regkey);

               // Set encoding setting.

          int index = (int) regkey.GetValue(strEncoding, 4);

          mieChecked.Checked = false;
          mieChecked = (MenuItemEncoding) miEncoding.MenuItems[index];
          mieChecked.Checked = true;

     }
     protected override void SaveRegistryInfo(RegistryKey regkey)
     {
          base.SaveRegistryInfo(regkey);
          regkey.SetValue(strEncoding, mieChecked.Index);
     }
                                                       // Utility routines
     protected void LoadFile(string strFileName)
     {
          StreamReader sr;

          try
          {
               sr = new StreamReader(strFileName);
          }
          catch (Exception exc)
          {
               MessageBox.Show(exc.Message, strProgName, 
                               MessageBoxButtons.OK,
                               MessageBoxIcon.Asterisk);
               return;
          }
          txtbox.Text = sr.ReadToEnd();
          sr.Close();

          this.strFileName = strFileName;
          MakeCaption();

          txtbox.SelectionStart = 0;
          txtbox.SelectionLength = 0;
          txtbox.Modified = false;
          txtbox.ClearUndo();
     }
     void SaveFile()
     {
          try
          {
               StreamWriter sw = new StreamWriter(strFileName, false,
                                                  mieChecked.Encoding);
               sw.Write(txtbox.Text);
               sw.Close();
          }
          catch (Exception exc)
          {
               MessageBox.Show(exc.Message, strProgName,
                               MessageBoxButtons.OK,
                               MessageBoxIcon.Asterisk);
               return;
          }
          txtbox.Modified = false;
     }
     bool SaveFileDlg()
     {
          SaveFileDialog sfd = new SaveFileDialog();
          
          if (strFileName != null && strFileName.Length > 1)
               sfd.FileName = strFileName;
          else
               sfd.FileName = "*.txt";

          sfd.Filter = strFilter;

          if (sfd.ShowDialog() == DialogResult.OK)
          {
               strFileName = sfd.FileName;
               SaveFile();
               MakeCaption();
               return true;
          }
          else
          {
               return false;       // Return values are for OkToTrash.
          }
     }
     protected void MakeCaption()
     {
          Text = strProgName + " - " + FileTitle();
     }
     protected string FileTitle()
     {
          return (strFileName != null && strFileName.Length > 1) ?
                         Path.GetFileName(strFileName) : "Untitled";
     }
     protected bool OkToTrash()
     {
          if (!txtbox.Modified)
               return true;

          DialogResult dr = 
                    MessageBox.Show("The text in the " + FileTitle() +
                                    " file has changed.\r\n\r\n" +
                                    "Do you want to save the changes?",
                                    strProgName,
                                    MessageBoxButtons.YesNoCancel,
                                    MessageBoxIcon.Exclamation);
          switch (dr)
          {
          case DialogResult.Yes:
               return SaveFileDlg();

          case DialogResult.No:
               return true;

          case DialogResult.Cancel:
               return false;
          }
          return false;
     }
}          
class MenuItemEncoding: MenuItem
{
     public Encoding Encoding;
}