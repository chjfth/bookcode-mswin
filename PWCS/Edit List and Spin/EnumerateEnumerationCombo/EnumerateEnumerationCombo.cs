//--------------------------------------------------------
// EnumerateEnumerationCombo.cs (R) 2001 by Charles Petzold
//--------------------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Windows.Forms;

class EnumerateEnumerationCombo: Form
{
     CheckBox cbHex;
     ComboBox comboLibrary, comboNamespace, comboEnumeration;
     TextBox  tbOutput;

     const string strRegKeyBase = 
          "Software\\ProgrammingWindowsWithCSharp\\EnumerateEnumerationCombo";

     public static void Main()
     {
          Application.Run(new EnumerateEnumerationCombo());
     }
     public EnumerateEnumerationCombo()
     {
          Text = "Enumerate Enumeration (Combo)";
          ClientSize = new Size(242, 164);

          Label label    = new Label();
          label.Parent   = this;
          label.Text     = "Library:";
          label.Location = new Point(8, 8);
          label.Size     = new Size(56, 8);
 
          comboLibrary               = new ComboBox();
          comboLibrary.Parent        = this;
          comboLibrary.DropDownStyle = ComboBoxStyle.DropDown;
          comboLibrary.Sorted        = true;
          comboLibrary.Location      = new Point(64, 8);
          comboLibrary.Size          = new Size(120, 12);
          comboLibrary.Anchor       |= AnchorStyles.Right;
          comboLibrary.TextChanged  += 
                         new EventHandler(ComboBoxLibraryOnTextChanged);

          label          = new Label();
          label.Parent   = this;
          label.Text     = "Namespace:";
          label.Location = new Point(8, 24);
          label.Size     = new Size(56, 8);
 
          comboNamespace               = new ComboBox();
          comboNamespace.Parent        = this;
          comboNamespace.DropDownStyle = ComboBoxStyle.DropDown;
          comboNamespace.Sorted        = true;
          comboNamespace.Location      = new Point(64, 24);
          comboNamespace.Size          = new Size(120, 12);
          comboNamespace.Anchor       |= AnchorStyles.Right;
          comboNamespace.TextChanged  +=
                         new EventHandler(ComboBoxNamespaceOnTextChanged);
          
          label          = new Label();
          label.Parent   = this;
          label.Text     = "Enumeration:";
          label.Location = new Point(8, 40);
          label.Size     = new Size(56, 8);
 
          comboEnumeration               = new ComboBox();
          comboEnumeration.Parent        = this;
          comboEnumeration.DropDownStyle = ComboBoxStyle.DropDown;
          comboEnumeration.Sorted        = true;
          comboEnumeration.Location      = new Point(64, 40);
          comboEnumeration.Size          = new Size(120, 12);
          comboEnumeration.Anchor       |= AnchorStyles.Right;
          comboEnumeration.TextChanged  +=
                         new EventHandler(ComboBoxEnumerationOnTextChanged);

          cbHex                 = new CheckBox();
          cbHex.Parent          = this;
          cbHex.Text            = "Hex";
          cbHex.Location        = new Point(192, 25);
          cbHex.Size            = new Size(40, 8);
          cbHex.Anchor          = AnchorStyles.Top | AnchorStyles.Right;
          cbHex.CheckedChanged += new EventHandler(CheckBoxOnCheckedChanged);

          tbOutput            = new TextBox();
          tbOutput.Parent     = this;
          tbOutput.ReadOnly   = true;
          tbOutput.Multiline  = true;
          tbOutput.ScrollBars = ScrollBars.Vertical;
          tbOutput.Location   = new Point(8, 56);
          tbOutput.Size       = new Size(226, 100);
          tbOutput.Anchor     = AnchorStyles.Left | AnchorStyles.Top |
                                AnchorStyles.Right | AnchorStyles.Bottom;

          AutoScaleBaseSize = new Size(4, 8);

               // Initialize display.

          FillComboBox(comboLibrary, strRegKeyBase);
          UpdateTextBox();
     }
     void ComboBoxLibraryOnTextChanged(object obj, EventArgs ea)
     {
          FillComboBox(comboNamespace, strRegKeyBase + "\\" + 
                                        comboLibrary.Text);

          ComboBoxNamespaceOnTextChanged(obj, ea);
     }
     void ComboBoxNamespaceOnTextChanged(object obj, EventArgs ea)
     {
          FillComboBox(comboEnumeration, strRegKeyBase + "\\" +
                                             comboLibrary.Text + "\\" +
                                                  comboNamespace.Text);

          ComboBoxEnumerationOnTextChanged(obj, ea);
     }
     void ComboBoxEnumerationOnTextChanged(object obj, EventArgs ea)
     {
          UpdateTextBox();
     }
     void CheckBoxOnCheckedChanged(object obj, EventArgs ea)
     {
          UpdateTextBox();
     }
     void UpdateTextBox()
     {
          if (EnumerateEnumeration.FillTextBox(tbOutput, comboLibrary.Text, 
               comboNamespace.Text, comboEnumeration.Text, cbHex.Checked))
          {
               if (!comboLibrary.Items.Contains(comboLibrary.Text))
                    comboLibrary.Items.Add(comboLibrary.Text);

               if (!comboNamespace.Items.Contains(comboNamespace.Text))
                    comboNamespace.Items.Add(comboNamespace.Text);

               if (!comboEnumeration.Items.Contains(comboEnumeration.Text))
                    comboEnumeration.Items.Add(comboEnumeration.Text);

               string strRegKey = strRegKeyBase + "\\" + 
                                        comboLibrary.Text + "\\" +
                                             comboNamespace.Text + "\\" +
                                                  comboEnumeration.Text;
               RegistryKey regkey = 
                              Registry.CurrentUser.OpenSubKey(strRegKey);

               if (regkey == null)
                    regkey = Registry.CurrentUser.CreateSubKey(strRegKey);

               regkey.Close();
          }
     }
     bool FillComboBox(ComboBox combo, string strRegKey) 
     {
          combo.Items.Clear();

          RegistryKey regkey = Registry.CurrentUser.OpenSubKey(strRegKey);

          if (regkey != null)
          {
               string[] astrSubKeys = regkey.GetSubKeyNames();
               regkey.Close();

               if (astrSubKeys.Length > 0)
               {
                    combo.Items.AddRange(astrSubKeys);
                    combo.SelectedIndex = 0;
                    return true;
               }
          }
          combo.Text = "";
          return false;
     }
}
