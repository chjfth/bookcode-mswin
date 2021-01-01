//-------------------------------------------
// BetterDialog.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BetterDialog: Form
{
     string strDisplay = "";

     public static void Main()
     {
          Application.Run(new BetterDialog());
     }
     public BetterDialog()
     {
          Text = "Better Dialog";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Dialog!", new EventHandler(MenuOnClick));
     }
     void MenuOnClick(object obj, EventArgs ea)
     {
          BetterDialogBox dlg = new BetterDialogBox();
          DialogResult    dr  = dlg.ShowDialog();

          strDisplay = "Dialog box terminated with " + dr + "!";
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          grfx.DrawString(strDisplay, Font, new SolidBrush(ForeColor), 0, 0);
     }
}
class BetterDialogBox: Form
{
     public BetterDialogBox()
     {
          Text = "Better Dialog Box";

               // Standard stuff for dialog boxes

          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MaximizeBox     = false;
          MinimizeBox     = false;
          ShowInTaskbar   = false;
          StartPosition   = FormStartPosition.Manual;
          Location        = ActiveForm.Location + 
                            SystemInformation.CaptionButtonSize +
                            SystemInformation.FrameBorderSize;

               // Create OK button.

          Button btn = new Button();
          btn.Parent   = this;
          btn.Text     = "OK";
          btn.Location = new Point(50, 50);
          btn.Size     = new Size (10 * Font.Height, 2 * Font.Height);
          btn.DialogResult = DialogResult.OK;

          AcceptButton = btn;

               // Create Cancel button.

          btn = new Button();
          btn.Parent   = this;
          btn.Text     = "Cancel";
          btn.Location = new Point(50, 100);
          btn.Size     = new Size (10 * Font.Height, 2 * Font.Height);
          btn.DialogResult = DialogResult.Cancel;

          CancelButton = btn;
     }
}
