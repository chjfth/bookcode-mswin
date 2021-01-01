//--------------------------------------------
// BitmapButtons.cs (R) 2001 by Charles Petzold
//--------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BitmapButtons: Form
{
     readonly int    cxBtn, cyBtn, dxBtn;
     readonly Button btnLarger, btnSmaller;

     public static void Main()
     {
          Application.Run(new BitmapButtons());
     }
     public BitmapButtons()
     {
          Text = "Bitmap Buttons";
          ResizeRedraw = true;

          dxBtn = Font.Height;

               // Create first button.

          btnLarger = new Button();
          btnLarger.Parent = this;
          btnLarger.Image  = new Bitmap(GetType(), 
                                   "BitmapButtons.LargerButton.bmp") ;

               // Calculate button dimensions based on image dimensions.

          cxBtn = btnLarger.Image.Width  + 8;
          cyBtn = btnLarger.Image.Height + 8;

          btnLarger.Size   = new Size(cxBtn, cyBtn);
          btnLarger.Click += new EventHandler(ButtonLargerOnClick);

               // Create second button.

          btnSmaller = new Button();
          btnSmaller.Parent = this;
          btnSmaller.Image  = new Bitmap(GetType(), 
                                   "BitmapButtons.SmallerButton.bmp");
          btnSmaller.Size   = new Size(cxBtn, cyBtn);
          btnSmaller.Click += new EventHandler(ButtonSmallerOnClick);

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
     void ButtonLargerOnClick(object obj, EventArgs ea)
     {
          Left   -= (int)(0.05 * Width);
          Top    -= (int)(0.05 * Height);
          Width  += (int)(0.10 * Width);
          Height += (int)(0.10 * Height);
     }
     void ButtonSmallerOnClick(object obj, EventArgs ea)
     {
          Left   += (int)(Width  / 22f);
          Top    += (int)(Height / 22f);
          Width  -= (int)(Width  / 11f);
          Height -= (int)(Height / 11f);
     }
}
