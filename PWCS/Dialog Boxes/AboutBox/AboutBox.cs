//---------------------------------------
// AboutBox.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class AboutBox: Form
{
     public static void Main()
     {
          Application.Run(new AboutBox());
     }
     public AboutBox()
     {
          Text = "About Box";
          Icon = new Icon(GetType(), "AboutBox.AforAbout.ico");

          Menu = new MainMenu();
          Menu.MenuItems.Add("&Help");
          Menu.MenuItems[0].MenuItems.Add("&About AboutBox...",
                                        new EventHandler(MenuAboutOnClick));
     }
     void MenuAboutOnClick(object obj, EventArgs ea)
     {
          AboutDialogBox dlg = new AboutDialogBox();
          dlg.ShowDialog();
     }
}
class AboutDialogBox: Form
{
     public AboutDialogBox()
     {
          Text = "About AboutBox";

          StartPosition   = FormStartPosition.CenterParent;
          FormBorderStyle = FormBorderStyle.FixedDialog;
          ControlBox      = false;
          MaximizeBox     = false;
          MinimizeBox     = false;
          ShowInTaskbar   = false;

          Label label1     = new Label();
          label1.Parent    = this;
          label1.Text      = " AboutBox Version 1.0 ";
          label1.Font      = new Font(FontFamily.GenericSerif, 24, 
                                      FontStyle.Italic);
          label1.AutoSize  = true;
          label1.TextAlign = ContentAlignment.MiddleCenter;

          Icon icon = new Icon(GetType(), "AboutBox.AforAbout.ico");

          PictureBox picbox = new PictureBox();
          picbox.Parent     = this;
          picbox.Image      = icon.ToBitmap();
          picbox.SizeMode   = PictureBoxSizeMode.AutoSize;
          picbox.Location   = new Point(label1.Font.Height / 2, 
                                        label1.Font.Height / 2);

          label1.Location  = new Point(picbox.Right,label1.Font.Height / 2);

          int iClientWidth = label1.Right;

          Label label2     = new Label();
          label2.Parent    = this;
          label2.Text      = "\x00A9 2001 by Charles Petzold";
          label2.Font      = new Font(FontFamily.GenericSerif, 16);
          label2.Location  = new Point(0, label1.Bottom + 
                                          label2.Font.Height);
          label2.Size      = new Size(iClientWidth, label2.Font.Height);
          label2.TextAlign = ContentAlignment.MiddleCenter;

          Button button   = new Button();
          button.Parent   = this;
          button.Text     = "OK";
          button.Size     = new Size(4 * button.Font.Height, 
                                     2 * button.Font.Height);
          button.Location = new Point((iClientWidth - button.Size.Width) / 2,
                                   label2.Bottom + 2 * button.Font.Height);

          button.DialogResult = DialogResult.OK;

          CancelButton = button;
          AcceptButton = button;

          ClientSize = new Size(iClientWidth, 
                                button.Bottom + 2 * button.Font.Height);
     }
}