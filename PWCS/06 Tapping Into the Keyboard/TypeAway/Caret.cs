//------------------------------------
// Caret.cs © 2001 by Charles Petzold
//------------------------------------
using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace Petzold.ProgrammingWindowsWithCSharp
{
     class Caret
     {
          [DllImport("user32.dll")]
          public static extern int CreateCaret(IntPtr hwnd, IntPtr hbm, 
                                               int cx, int cy);
          [DllImport("user32.dll")]
          public static extern int DestroyCaret();

          [DllImport("user32.dll")]
          public static extern int SetCaretPos(int x, int y);

          [DllImport("user32.dll")]
          public static extern int ShowCaret(IntPtr hwnd);

          [DllImport("user32.dll")]
          public static extern int HideCaret(IntPtr hwnd);
                                                            // Fields
          Control ctrl;
          Size    size;
          Point   ptPos;
          bool    bVisible;
                                                            // Constructors
               // Don't allow default constructor.

          private Caret()
          {
          }
               // Only allowable constructor has Control argument.

          public Caret(Control ctrl)
          {
               this.ctrl = ctrl;
               Position  = Point.Empty;
               Size      = new Size(1, ctrl.Font.Height);

               Control.GotFocus  += new EventHandler(ControlOnGotFocus);
               Control.LostFocus += new EventHandler(ControlOnLostFocus);

                    // If the control already has focus, create the caret.

               if (ctrl.Focused)
                    ControlOnGotFocus(ctrl, new EventArgs());
          }
                                                            // Properties
          public Control Control
          {
               get 
               {
                    return ctrl;
               }
          }
          public Size Size
          {
               get 
               {
                    return size;
               }
               set
               {
                    size = value;
               }
          }
          public Point Position
          {
               get
               {
                    return ptPos;
               }
               set
               {
                    ptPos = value;
                    SetCaretPos(ptPos.X, ptPos.Y);
               }
          }
          public bool Visibility
          {
               get
               {
                    return bVisible;
               }
               set
               {
                    if (bVisible = value)
                         ShowCaret(Control.Handle);
                    else
                         HideCaret(Control.Handle);
               }
          }
                                                            // Methods
          public void Show()
          {
               Visibility = true;
          }
          public void Hide()
          {
               Visibility = false;
          }
          public void Dispose()
          {
                    // If the control has focus, destroy the caret.

               if (ctrl.Focused)
                    ControlOnLostFocus(ctrl, new EventArgs());

               Control.GotFocus  -= new EventHandler(ControlOnGotFocus);
               Control.LostFocus -= new EventHandler(ControlOnLostFocus);
          }
                                                            // Event handlers
          void ControlOnGotFocus(object obj, EventArgs ea)
          {
               CreateCaret(Control.Handle, IntPtr.Zero, 
                           Size.Width, Size.Height);
               SetCaretPos(Position.X, Position.Y);
               Show();
          }
          void ControlOnLostFocus(object obj, EventArgs ea)
          {
               Hide();
               DestroyCaret();
          }
     }
}
