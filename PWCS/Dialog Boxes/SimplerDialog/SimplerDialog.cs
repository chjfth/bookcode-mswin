//--------------------------------------------
// SimplerDialog.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimplerDialog: Form
{
     string strDisplay = "";

     public static void Main()
     {
          Application.Run(new SimplerDialog());
     }
     public SimplerDialog()
     {
          Text = "Simpler Dialog";

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Dialog!", new EventHandler(MenuOnClick));
     }
     void MenuOnClick(object obj, EventArgs ea)
     {
          SimplerDialogBox dlg = new SimplerDialogBox();
          DialogResult     dr  = dlg.ShowDialog();

          strDisplay = "Dialog box terminated with " + dr + "!";
          Invalidate();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          grfx.DrawString(strDisplay, Font, new SolidBrush(ForeColor), 0, 0);
     }
}
class SimplerDialogBox: Form
{
     public SimplerDialogBox()
     {
          Text = "Simpler Dialog Box";

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
          btn.DialogResult = DialogResult.OK;

               // Create Cancel button.

          btn = new Button();
          btn.Parent   = this;
          btn.Text     = "Cancel";
          btn.Location = new Point(50, 100);
          btn.Size     = new Size (10 * Font.Height, 2 * Font.Height);
          btn.DialogResult = DialogResult.Cancel;
     }
}
