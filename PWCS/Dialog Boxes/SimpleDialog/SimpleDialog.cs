//-------------------------------------------
// SimpleDialog.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleDialog: Form
{
     string strDisplay = "";

     public static void Main()
     {
          Application.Run(new SimpleDialog());
     }
     public SimpleDialog()
     {
          Text = "Simple Dialog";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Dialog!", new EventHandler(MenuOnClick));
     }
     void MenuOnClick(object obj, EventArgs ea)
     {
          SimpleDialogBox dlg = new SimpleDialogBox();

          dlg.ShowDialog();

          strDisplay = "Dialog box terminated with " + 
                            dlg.DialogResult + "!";
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          grfx.DrawString(strDisplay, Font, new SolidBrush(ForeColor), 0, 0);
     }
}
class SimpleDialogBox: Form
{
     public SimpleDialogBox()
     {
          Text = "Simple Dialog Box";

               // Standard stuff for dialog boxes

          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MaximizeBox     = false;
          MinimizeBox     = false;
          ShowInTaskbar   = false;

               // Create OK button.

          Button btn = new Button();
          btn.Parent   = this;
          btn.Text     = "OK";
          btn.Location = new Point(50, 50);
          btn.Size     = new Size (10 * Font.Height, 2 * Font.Height);
          btn.Click   += new EventHandler(ButtonOkOnClick);

               // Create Cancel button.

          btn = new Button();
          btn.Parent   = this;
          btn.Text     = "Cancel";
          btn.Location = new Point(50, 100);
          btn.Size     = new Size (10 * Font.Height, 2 * Font.Height);
          btn.Click   += new EventHandler(ButtonCancelOnClick);
     }
     void ButtonOkOnClick(object obj, EventArgs ea)
     {
          DialogResult = DialogResult.OK;
     }
     void ButtonCancelOnClick(object obj, EventArgs ea)
     {
          DialogResult = DialogResult.Cancel;
     }
}
