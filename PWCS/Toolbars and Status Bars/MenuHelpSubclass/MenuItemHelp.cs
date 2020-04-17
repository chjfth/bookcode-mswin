//-------------------------------------------
// MenuItemHelp.cs © 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MenuItemHelp: MenuItem
{
                                                  // Private fields
     StatusBarPanel sbpHelpPanel;
     string         strHelpText;
                                                  // Constructors
     public MenuItemHelp()
     {
     }
     public MenuItemHelp(string strText): base(strText)
     {
     }
                                                  // Properties
     public StatusBarPanel HelpPanel
     {
          get { return sbpHelpPanel; }
          set { sbpHelpPanel = value; }
     }
     public string HelpText
     {
          get { return strHelpText; }
          set { strHelpText = value; }
     }
                                                  // Method override
     protected override void OnSelect(EventArgs ea)
     {
          base.OnSelect(ea);

          if (HelpPanel != null)
               HelpPanel.Text = HelpText;
     }
}