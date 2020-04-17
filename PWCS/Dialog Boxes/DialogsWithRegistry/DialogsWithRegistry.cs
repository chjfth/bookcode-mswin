//--------------------------------------------------
// DialogsWithRegistry.cs © 2001 by Charles Petzold
//--------------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Windows.Forms;

class DialogsWithRegistry: BetterFontAndColorDialogs
{
     const string strRegKey = 
                         "Software\\ProgrammingsWindowsWithCSharp\\DialogsWithRegistry";
     const string strFontFace  = "FontFace";
     const string strFontSize  = "FontSize";
     const string strFontStyle = "FontStyle";
     const string strForeColor = "ForeColor";
     const string strBackColor = "BackColor";
     const string strCustomClr = "CustomColor";

     public new static void Main()
     {
          Application.Run(new DialogsWithRegistry());
     }
     public DialogsWithRegistry()
     {
          Text = "Font and Color Dialogs with Registry";

          RegistryKey regkey = Registry.CurrentUser.OpenSubKey(strRegKey);

          if (regkey != null)
          {
               Font = new Font((string) regkey.GetValue(strFontFace),
                               float.Parse(
                                   (string) regkey.GetValue(strFontSize)),
                               (FontStyle) regkey.GetValue(strFontStyle));

               ForeColor = Color.FromArgb(
                              (int) regkey.GetValue(strForeColor));

               BackColor = Color.FromArgb(
                              (int) regkey.GetValue(strBackColor));

               int[] aiColors = new int[16];

               for (int i = 0; i < 16; i++)
                    aiColors[i] = (int) regkey.GetValue(strCustomClr + i);

               clrdlg.CustomColors = aiColors;

               regkey.Close();
          }
     }
     protected override void OnClosed(EventArgs ea)
     {
          RegistryKey regkey = 
                         Registry.CurrentUser.OpenSubKey(strRegKey, true);
          if (regkey == null)
               regkey = Registry.CurrentUser.CreateSubKey(strRegKey);

          regkey.SetValue(strFontFace,  Font.Name);
          regkey.SetValue(strFontSize,  Font.SizeInPoints.ToString());
          regkey.SetValue(strFontStyle, (int) Font.Style);
          regkey.SetValue(strForeColor, ForeColor.ToArgb());
          regkey.SetValue(strBackColor, BackColor.ToArgb());

          for (int i = 0; i < 16; i++)
               regkey.SetValue(strCustomClr + i, clrdlg.CustomColors[i]);

          regkey.Close();
     }
}