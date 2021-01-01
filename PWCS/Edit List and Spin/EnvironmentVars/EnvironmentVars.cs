//----------------------------------------------
// EnvironmentVars.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

class EnvironmentVars: Form
{
     Label label;

     public static void Main()
     {
          Application.Run(new EnvironmentVars());
     }
     public EnvironmentVars()
     {
          Text = "Environment Variables";

               // Create Label control.

          label = new Label();
          label.Parent   = this;
          label.Anchor   = AnchorStyles.Left | AnchorStyles.Right;
          label.Location = new Point(Font.Height, Font.Height);
          label.Size     = new Size(ClientSize.Width - 2 * Font.Height, 
                                    Font.Height);

               // Create ListBox control.

          ListBox listbox  = new ListBox();
          listbox.Parent   = this;
          listbox.Location = new Point(Font.Height, 3 * Font.Height);
          listbox.Size     = new Size(12 * Font.Height, 8 * Font.Height);
          listbox.Sorted   = true;
          listbox.SelectedIndexChanged += 
                         new EventHandler(ListBoxOnSelectedIndexChanged);

               // Set environment strings in ListBox control.

          IDictionary dict = Environment.GetEnvironmentVariables();
          string[]    astr = new String[dict.Keys.Count];

          dict.Keys.CopyTo(astr, 0);
          listbox.Items.AddRange(astr);
          listbox.SelectedIndex = 0;
     }
     void ListBoxOnSelectedIndexChanged(object obj, EventArgs ea)
     {
          ListBox listbox = (ListBox) obj;
          string  strItem = (string) listbox.SelectedItem;

          label.Text = Environment.GetEnvironmentVariable(strItem);
     }
}