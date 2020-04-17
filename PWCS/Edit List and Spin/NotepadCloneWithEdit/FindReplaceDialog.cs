//------------------------------------------------
// FindReplaceDialog.cs © 2001 by Charles Petzold
//------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class FindDialog: FindReplaceDialog
{
     public FindDialog()
     {
          Text = "Find";
 
          labelReplace.Visible = false;
          txtboxReplace.Visible = false;
          btnReplace.Visible = false;
          btnReplaceAll.Visible = false;
          btnCancel.Location = btnReplace.Location;
     }
}
class ReplaceDialog: FindReplaceDialog
{
     public ReplaceDialog()
     {
          Text = "Replace";

          grpboxDirection.Visible = false;
     }
}
abstract class FindReplaceDialog: Form
{
                                                       // Fields
     protected Label       labelFind, labelReplace;
     protected TextBox     txtboxFind, txtboxReplace;
     protected CheckBox    chkboxMatchCase;
     protected GroupBox    grpboxDirection;
     protected RadioButton radiobtnUp, radiobtnDown;
     protected Button      btnFindNext, btnReplace, btnReplaceAll, btnCancel;

                                                       // Public events
     public event EventHandler FindNext;
     public event EventHandler Replace;
     public event EventHandler ReplaceAll;
     public event EventHandler CloseDlg;
                                                       // Constructor
     public FindReplaceDialog()
     {
          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MinimizeBox     = false;
          MaximizeBox     = false;
          ShowInTaskbar   = false;
          StartPosition   = FormStartPosition.Manual;
          Location        = ActiveForm.Location +
                            SystemInformation.CaptionButtonSize +
                            SystemInformation.FrameBorderSize;

          labelFind = new Label();
          labelFind.Parent = this;
          labelFind.Text = "Fi&nd what:";
          labelFind.Location = new Point(8, 8);
          labelFind.Size = new Size(64, 8);

          txtboxFind = new TextBox();
          txtboxFind.Parent = this;
          txtboxFind.Location = new Point(72, 8);
          txtboxFind.Size = new Size(136, 8);
          txtboxFind.TextChanged += 
                         new EventHandler(TextBoxFindOnTextChanged);

          labelReplace = new Label();
          labelReplace.Parent = this;
          labelReplace.Text = "Re&place with:";
          labelReplace.Location = new Point(8, 24);
          labelReplace.Size = new Size(64, 8);

          txtboxReplace = new TextBox();
          txtboxReplace.Parent = this;
          txtboxReplace.Location = new Point(72, 24);
          txtboxReplace.Size = new Size(136, 8);
          
          chkboxMatchCase = new CheckBox();
          chkboxMatchCase.Parent = this;
          chkboxMatchCase.Text = "Match &case";
          chkboxMatchCase.Location = new Point(8, 50); // 48);
          chkboxMatchCase.Size = new Size(64, 8);

          grpboxDirection = new GroupBox();
          grpboxDirection.Parent = this;
          grpboxDirection.Text = "Direction";
          grpboxDirection.Location = new Point(100, 40);
          grpboxDirection.Size = new Size(96, 24);

          radiobtnUp = new RadioButton();
          radiobtnUp.Parent = grpboxDirection;
          radiobtnUp.Text = "&Up";
          radiobtnUp.Location = new Point(8, 8);
          radiobtnUp.Size = new Size(32, 12);

          radiobtnDown = new RadioButton();
          radiobtnDown.Parent = grpboxDirection;
          radiobtnDown.Text = "&Down";
          radiobtnDown.Location = new Point(40, 8);
          radiobtnDown.Size = new Size(40, 12);

          btnFindNext = new Button();
          btnFindNext.Parent   = this;
          btnFindNext.Text     = "&Find Next";
          btnFindNext.Enabled  = false;
          btnFindNext.Location = new Point(216, 4);
          btnFindNext.Size     = new Size(64, 16);
          btnFindNext.Click   += new EventHandler(ButtonFindNextOnClick);

          btnReplace = new Button();
          btnReplace.Parent   = this;
          btnReplace.Text     = "&Replace";
          btnReplace.Enabled  = false;
          btnReplace.Location = new Point(216, 24);
          btnReplace.Size     = new Size(64, 16);
          btnReplace.Click   += new EventHandler(ButtonReplaceOnClick);
          
          btnReplaceAll = new Button();
          btnReplaceAll.Parent   = this;
          btnReplaceAll.Text     = "Replace &All";
          btnReplaceAll.Enabled  = false;
          btnReplaceAll.Location = new Point(216, 44);
          btnReplaceAll.Size     = new Size(64, 16);
          btnReplaceAll.Click   += new EventHandler(ButtonReplaceAllOnClick);

          btnCancel = new Button();
          btnCancel.Parent   = this;
          btnCancel.Text     = "Cancel";
          btnCancel.Location = new Point(216, 64);
          btnCancel.Size     = new Size(64, 16);
          btnCancel.Click   += new EventHandler(ButtonCancelOnClick);
          CancelButton = btnCancel;

          ClientSize = new Size(288, 84);
          AutoScaleBaseSize = new Size(4, 8);
     }
                                                       // Properties
     public string FindText
     {
          set { txtboxFind.Text = value; }
          get { return txtboxFind.Text; }
     }
     public string ReplaceText
     {
          set { txtboxReplace.Text = value; }
          get { return txtboxReplace.Text; }
     }
     public bool MatchCase
     {
          set { chkboxMatchCase.Checked = value; }
          get { return chkboxMatchCase.Checked; }
     }
     public bool FindDown
     {
          set 
          { 
               if (value)
                    radiobtnUp.Checked = true;
               else
                    radiobtnDown.Checked = true;
          }
          get { return radiobtnDown.Checked; }
     }
                                                       // Event handlers
     void TextBoxFindOnTextChanged(object obj, EventArgs ea)
     {
          btnFindNext.Enabled =
          btnReplace.Enabled =
          btnReplaceAll.Enabled = txtboxFind.Text.Length > 0;
     }
     void ButtonFindNextOnClick(object obj, EventArgs ea)
     {
          if (FindNext != null)
               FindNext(this, EventArgs.Empty);
     }
     void ButtonReplaceOnClick(object obj, EventArgs ea)
     {
          if (Replace != null)
               Replace(this, EventArgs.Empty);
     }
     void ButtonReplaceAllOnClick(object obj, EventArgs ea)
     {
          if (ReplaceAll != null)
               ReplaceAll(this, EventArgs.Empty);
     }
     void ButtonCancelOnClick(object obj, EventArgs ea)
     {
          if (CloseDlg != null)
               CloseDlg(this, EventArgs.Empty);

          Close();
     }
}
