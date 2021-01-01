//-------------------------------------------------------
// NotepadCloneWithRegistry.cs (R) 2001 by Charles Petzold
//-------------------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Windows.Forms;

class NotepadCloneWithRegistry: NotepadCloneNoMenu
{
     Rectangle        rectNormal;
     protected string strProgName;
     string           strRegKey    = "Software\\ProgrammingWindowsWithCSharp\\";
     const string     strWinState  = "WindowState";
     const string     strLocationX = "LocationX";
     const string     strLocationY = "LocationY";
     const string     strWidth     = "Width";
     const string     strHeight    = "Height";

     public new static void Main()
     {
          Application.Run(new NotepadCloneWithRegistry());
     }
     public NotepadCloneWithRegistry()
     {
          Text = strProgName = "Notepad Clone with Registry";
          rectNormal = DesktopBounds;
     }
     protected override void OnMove(EventArgs ea)
     {
          base.OnMove(ea);

          if (WindowState == FormWindowState.Normal)
               rectNormal = DesktopBounds;
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          if (WindowState == FormWindowState.Normal)
               rectNormal = DesktopBounds;
     }
     protected override void OnLoad(EventArgs ea)
     {
          base.OnLoad(ea);

               // Construct complete registry key.

          strRegKey = strRegKey + strProgName;

               // Load registry information.

          RegistryKey regkey = Registry.CurrentUser.OpenSubKey(strRegKey);

          if (regkey != null)
          {
               LoadRegistryInfo(regkey);
               regkey.Close();
          }
     }
     protected override void OnClosed(EventArgs ea)
     {
          base.OnClosed(ea);

               // Save registry information.

          RegistryKey regkey = 
                         Registry.CurrentUser.OpenSubKey(strRegKey, true);

          if (regkey == null)
               regkey = Registry.CurrentUser.CreateSubKey(strRegKey);

          SaveRegistryInfo(regkey);
          regkey.Close();
     }
     protected virtual void SaveRegistryInfo(RegistryKey regkey)
     {
          regkey.SetValue(strWinState,  (int) WindowState);
          regkey.SetValue(strLocationX, rectNormal.X);
          regkey.SetValue(strLocationY, rectNormal.Y);
          regkey.SetValue(strWidth,     rectNormal.Width);
          regkey.SetValue(strHeight,    rectNormal.Height);
     }
     protected virtual void LoadRegistryInfo(RegistryKey regkey)
     {
          int x  = (int) regkey.GetValue(strLocationX, 100);
          int y  = (int) regkey.GetValue(strLocationY, 100);
          int cx = (int) regkey.GetValue(strWidth, 300);
          int cy = (int) regkey.GetValue(strHeight, 300);

          rectNormal = new Rectangle(x, y, cx, cy);

               // Adjust rectangle for any change in desktop size.
          
          Rectangle rectDesk = SystemInformation.WorkingArea;

          rectNormal.Width  = Math.Min(rectNormal.Width,  rectDesk.Width);
          rectNormal.Height = Math.Min(rectNormal.Height, rectDesk.Height);
          rectNormal.X -= Math.Max(rectNormal.Right  - rectDesk.Right,  0);
          rectNormal.Y -= Math.Max(rectNormal.Bottom - rectDesk.Bottom, 0);

               // Set form properties.

          DesktopBounds = rectNormal;
          WindowState = (FormWindowState) regkey.GetValue(strWinState, 0);
     }
}
