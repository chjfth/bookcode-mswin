//-----------------------------------------
// TwoButtons.cs © 2001 by Charles Petzold
//-----------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class TwoButtons: Form
{
     readonly Button btnLarger, btnSmaller;
     readonly int    cxBtn, cyBtn, dxBtn;

     public static void Main()
     {
          Application.Run(new TwoButtons());
     }
     public TwoButtons()
     {
          Text = "Two Buttons";
          ResizeRedraw = true;

          cxBtn = 5 * Font.Height;
          cyBtn = 2 * Font.Height;
          dxBtn =     Font.Height;

          btnLarger = new Button();
          btnLarger.Parent = this;
          btnLarger.Text   = "&Larger";
          btnLarger.Size   = new Size(cxBtn, cyBtn);
          btnLarger.Click += new EventHandler(ButtonOnClick);

          btnSmaller = new Button();
          btnSmaller.Parent = this;
          btnSmaller.Text   = "&Smaller";
          btnSmaller.Size   = new Size(cxBtn, cyBtn);
          btnSmaller.Click += new EventHandler(ButtonOnClick);

		 OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);

          btnLarger.Location =
                         new Point(ClientSize.Width / 2 - cxBtn - dxBtn / 2,
                                  (ClientSize.Height - cyBtn) / 2);
          btnSmaller.Location =
                         new Point(ClientSize.Width / 2 + dxBtn / 2,
                                  (ClientSize.Height - cyBtn) / 2);
     }
     void ButtonOnClick(object obj, EventArgs ea)
     {
          Button btn = (Button) obj;

          if (btn == btnLarger)
          {
               Left   -= (int)(0.05 * Width);
               Top    -= (int)(0.05 * Height);
               Width  += (int)(0.10 * Width);
               Height += (int)(0.10 * Height);
          }
          else
          {
               Left   += (int)(Width  / 22f);
               Top    += (int)(Height / 22f);
               Width  -= (int)(Width  / 11f);
               Height -= (int)(Height / 11f);
          }
     }
}
