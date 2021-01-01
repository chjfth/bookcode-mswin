//-----------------------------------------------------
// PrinterSelectionDialog.cs (R) 2001 by Charles Petzold
//-----------------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class PrinterSelectionDialog: Form
{
     ComboBox combo;

     public PrinterSelectionDialog()
     {
          Text = "Select Printer";

          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MaximizeBox     = false;
          MinimizeBox     = false;
          ShowInTaskbar   = false;
          StartPosition   = FormStartPosition.Manual;
          Location        = ActiveForm.Location +
                                   SystemInformation.CaptionButtonSize +
                                   SystemInformation.FrameBorderSize;

          Label label    = new Label();
          label.Parent   = this;
          label.Text     = "Printer:";
          label.Location = new Point(8, 8);
          label.Size     = new Size(40, 8);

          combo = new ComboBox();
          combo.Parent   = this;
          combo.DropDownStyle = ComboBoxStyle.DropDownList;
          combo.Location = new Point(48, 8);
          combo.Size     = new Size(144, 8);

               // Add the installed printers to the combo box.

          foreach (string str in PrinterSettings.InstalledPrinters)
               combo.Items.Add(str);

          Button btn   = new Button();
          btn.Parent   = this;
          btn.Text     = "OK";
          btn.Location = new Point(40, 32);
          btn.Size     = new Size(40, 16);
          btn.DialogResult = DialogResult.OK;

          AcceptButton = btn;

          btn  = new Button();
          btn.Parent = this;
          btn.Text = "Cancel";
          btn.Location = new Point(120, 32);
          btn.Size   = new Size(40, 16);
          btn.DialogResult = DialogResult.Cancel;

          CancelButton = btn;

          ClientSize = new Size(200, 56);
          AutoScaleBaseSize = new Size(4, 8);
     }
     public string PrinterName
     {
          set { combo.SelectedItem = value; }
          get { return (string) combo.SelectedItem; }
     }
}