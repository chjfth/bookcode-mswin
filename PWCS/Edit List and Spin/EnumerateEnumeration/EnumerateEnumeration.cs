//---------------------------------------------------
// EnumerateEnumeration.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Reflection;           // For the Assembly class
using System.Text;                 // For the StringBuilder class
using System.Windows.Forms;

class EnumerateEnumeration: Form
{
     Button   button;
     TextBox  tbLibrary, tbNamespace, tbEnumeration, tbOutput;
     CheckBox cbHex;

     public static void Main()
     {
          Application.Run(new EnumerateEnumeration());
     }
     public EnumerateEnumeration()
     {
          Text = "Enumerate Enumeration";
          ClientSize = new Size(242, 164);

          Label label    = new Label();
          label.Parent   = this;
          label.Text     = "Library:";
          label.Location = new Point(8, 8);
          label.Size     = new Size(56, 8);
 
          tbLibrary          = new TextBox();
          tbLibrary.Parent   = this;
          tbLibrary.Text     = "system.windows.forms";
          tbLibrary.Location = new Point(64, 8);
          tbLibrary.Size     = new Size(120, 12);
          tbLibrary.Anchor  |= AnchorStyles.Right;

          ToolTip tooltip = new ToolTip();
          tooltip.SetToolTip(tbLibrary, 
                                   "Enter the name of a .NET dynamic\n" +
                                   "link libary, such as 'mscorlib',\n" + 
                                   "'system.windows.forms', or\n" +
                                   "'system.drawing'.");

          label          = new Label();
          label.Parent   = this;
          label.Text     = "Namespace:";
          label.Location = new Point(8, 24);
          label.Size     = new Size(56, 8);
 
          tbNamespace          = new TextBox();
          tbNamespace.Parent   = this;
          tbNamespace.Text     = "System.Windows.Forms";
          tbNamespace.Location = new Point(64, 24);
          tbNamespace.Size     = new Size(120, 12);
          tbNamespace.Anchor  |= AnchorStyles.Right;

          tooltip.SetToolTip(tbNamespace, 
                                   "Enter the name of a namespace\n" +
                                   "within the library, such as\n" +
                                   "'System', 'System.IO',\n" +
                                   "'System.Drawing',\n" +
                                   "'System.Drawing.Drawing2D',\n" +
                                   "or 'System.Windows.Forms'.");
                         
          label          = new Label();
          label.Parent   = this;
          label.Text     = "Enumeration:";
          label.Location = new Point(8, 40);
          label.Size     = new Size(56, 8);
 
          tbEnumeration          = new TextBox();
          tbEnumeration.Parent   = this;
          tbEnumeration.Text     = "ScrollBars";
          tbEnumeration.Location = new Point(64, 40);
          tbEnumeration.Size     = new Size(120, 12);
          tbEnumeration.Anchor  |= AnchorStyles.Right;

          tooltip.SetToolTip(tbEnumeration, 
                                   "Enter the name of an enumeration\n" +
                                   "defined in the namespace.");

          cbHex                 = new CheckBox();
          cbHex.Parent          = this;
          cbHex.Text            = "Hex";
          cbHex.Location        = new Point(192, 16);
          cbHex.Size            = new Size(40, 8);
          cbHex.Anchor          = AnchorStyles.Top | AnchorStyles.Right;
          cbHex.CheckedChanged += new EventHandler(CheckBoxOnCheckedChanged);

          tooltip.SetToolTip(cbHex, "Check this box to display the\n" +
                                    "enumeration values in hexadecimal.");

          button          = new Button();
          button.Parent   = this;
          button.Text     = "OK";
          button.Location = new Point(192, 32);
          button.Size     = new Size(40, 16);
          button.Anchor   = AnchorStyles.Top | AnchorStyles.Right;
          button.Click   += new EventHandler(ButtonOkOnClick);

          AcceptButton = button;

          tooltip.SetToolTip(button, 
                                   "Click this button to display results.");
          
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

               // Initialize the display.

          ButtonOkOnClick(button, EventArgs.Empty);
     }
     void CheckBoxOnCheckedChanged(object sender, EventArgs ea)
     {
          button.PerformClick();
     }
     void ButtonOkOnClick(object sender, EventArgs ea)
     {
          FillTextBox(tbOutput, tbLibrary.Text, tbNamespace.Text, 
                      tbEnumeration.Text, cbHex.Checked);
     }
     public static bool FillTextBox(TextBox tbOutput, string strLibrary,
                                    string strNamespace, 
                                    string strEnumeration, bool bHexadecimal)
     {
		  string strEnumText = strNamespace + "." + strEnumeration;
 		  string strAssembly;
		 
		  try
		  {
		 	   strAssembly = 
				   Assembly.LoadWithPartialName(strLibrary).FullName;
		  }
		  catch
		  {
			  return false;
		  }
		  string strFullText = strEnumText + "," + strAssembly;

               // Get the type of the enum.

          Type type = Type.GetType(strFullText, false, true);

          if(type == null) 
          {
               tbOutput.Text = "\"" + strFullText + 
                               "\" is not a valid type.";
               return false;
          }
          else if(!type.IsEnum)
          {
               tbOutput.Text = "\"" + strEnumText + 
                               "\" is a valid type but not an enum.";
               return false;
          }

               // Get all the members in that enum.

          string[] astrMembers = Enum.GetNames(type);
          Array    arr         = Enum.GetValues(type);
          object[] aobjMembers = new object[arr.Length];

          arr.CopyTo(aobjMembers, 0);

               // Create a StringBuilder for the text.

          StringBuilder sb = new StringBuilder();

               // Append the enumeration name and headings.

          sb.Append(strEnumeration);
          sb.Append(" Enumeration\r\nMember\tValue\r\n");

               // Append the text rendition and the actual numeric values.

          for (int i = 0; i < astrMembers.Length; i++)
          {
               sb.Append(astrMembers[i]);
               sb.Append("\t");

               if (bHexadecimal)
                    sb.Append("0x" + Enum.Format(type, aobjMembers[i], "X"));
               else
                    sb.Append(Enum.Format(type, aobjMembers[i], "D"));

               sb.Append("\r\n");
          }
               // Append some other information.

          sb.Append("\r\nTotal = " + astrMembers.Length + "\r\n");
          sb.Append("\r\n" + type.AssemblyQualifiedName + "\r\n");

               // Set the text box Text property from the StringBuilder.

          tbOutput.Text = sb.ToString();
          tbOutput.SelectionLength = 0;
          return true;
     }
}
