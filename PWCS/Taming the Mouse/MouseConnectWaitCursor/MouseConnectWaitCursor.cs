// ----------------------------------------------------
// MouseConnectWaitCursor.cs © 2001 by Charles Petzold
// ----------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MouseConnectWaitCursor: MouseConnect
{
     public new static void Main()
     {
          Application.Run(new MouseConnectWaitCursor());
     }
     public MouseConnectWaitCursor()
     {
          Text = "Mouse Connect with Wait Cursor";
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Cursor.Current = Cursors.WaitCursor;
          Cursor.Show();

          base.OnPaint(pea);

          Cursor.Hide();
          Cursor.Current = Cursors.Arrow;
     }
}
