//-----------------------------------------
// KeyExamine.cs © 2001 by Charles Petzold
//-----------------------------------------
using System;	
using System.Drawing;
using System.Windows.Forms;

public class KeyExamine: Form 
{
     public static void Main() 
     {
     	Application.Run(new KeyExamine());
     }
          // Enum and struct definitions for storage of key events

     enum EventType
     {
          None,
          KeyDown,
          KeyUp,
          KeyPress
     }
     struct KeyEvent
     {
          public EventType evttype;
          public EventArgs evtargs;
     }
          // Storage of key events

     const int  iNumLines    = 25;
     int        iNumValid    = 0;
     int        iInsertIndex = 0;
     KeyEvent[] akeyevt = new KeyEvent[iNumLines];

          // Text positioning

     int xEvent, xChar, xCode, xMods, xData, 
         xShift, xCtrl, xAlt, xRight;
     
     public KeyExamine() 
     {
          Text = "Key Examine";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;

          xEvent = 0;
          xChar  = xEvent + 5 * Font.Height;
          xCode  = xChar  + 5 * Font.Height;
          xMods  = xCode  + 8 * Font.Height;
          xData  = xMods  + 8 * Font.Height;
          xShift = xData  + 8 * Font.Height;
          xCtrl  = xShift + 5 * Font.Height;
          xAlt   = xCtrl  + 5 * Font.Height;
          xRight = xAlt   + 5 * Font.Height;

          ClientSize  = new Size(xRight, Font.Height * (iNumLines + 1));
          FormBorderStyle = FormBorderStyle.Fixed3D;
          MaximizeBox = false;
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          akeyevt[iInsertIndex].evttype = EventType.KeyDown;
          akeyevt[iInsertIndex].evtargs = kea;
          OnKey();
     }
     protected override void OnKeyUp(KeyEventArgs kea)
     {
          akeyevt[iInsertIndex].evttype = EventType.KeyUp;
          akeyevt[iInsertIndex].evtargs = kea;
          OnKey();
     }
     protected override void OnKeyPress(KeyPressEventArgs kpea)
     {
          akeyevt[iInsertIndex].evttype = EventType.KeyPress;
          akeyevt[iInsertIndex].evtargs = kpea;
          OnKey();
     }
     void OnKey()
     {
          if(iNumValid < iNumLines)
          {
               Graphics grfx = CreateGraphics();
               DisplayKeyInfo(grfx, iInsertIndex, iInsertIndex);
               grfx.Dispose();
          }
          else
          {
               ScrollLines();
          }
          iInsertIndex = (iInsertIndex + 1) % iNumLines;
          iNumValid = Math.Min(iNumValid + 1, iNumLines);
     }
     protected virtual void ScrollLines()
     {
          Rectangle rect = new Rectangle(0, Font.Height, 
                                         ClientSize.Width,
                                         ClientSize.Height - Font.Height);

               // I wish I could scroll here!

          Invalidate(rect);
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          BoldUnderline(grfx, "Event",     xEvent, 0);
          BoldUnderline(grfx, "KeyChar",   xChar,  0);
          BoldUnderline(grfx, "KeyCode",   xCode,  0);
          BoldUnderline(grfx, "Modifiers", xMods,  0);
          BoldUnderline(grfx, "KeyData",   xData,  0);
          BoldUnderline(grfx, "Shift",     xShift, 0);
          BoldUnderline(grfx, "Control",   xCtrl,  0);
          BoldUnderline(grfx, "Alt",       xAlt,   0);

          if(iNumValid < iNumLines)
          {
               for (int i = 0; i < iNumValid; i++)
                    DisplayKeyInfo(grfx, i, i);
          }
          else
          {
               for (int i = 0; i < iNumLines; i++)
                    DisplayKeyInfo(grfx, i,(iInsertIndex + i) % iNumLines);
          }
     }
     void BoldUnderline(Graphics grfx, string str, int x, int y)
     {
               // Draw the text bold.

          Brush brush = new SolidBrush(ForeColor);
          grfx.DrawString(str, Font, brush, x, y);
          grfx.DrawString(str, Font, brush, x + 1, y);

               // Underline the text.

          SizeF sizef = grfx.MeasureString(str, Font);
          grfx.DrawLine(new Pen(ForeColor), x, y + sizef.Height,
                                        x + sizef.Width, y + sizef.Height);
     }
     void DisplayKeyInfo(Graphics grfx, int y, int i)
     {
          Brush br = new SolidBrush(ForeColor);
          y = (1 + y) * Font.Height;    // Convert y to pixel coordinate

          grfx.DrawString(akeyevt[i].evttype.ToString(), 
                          Font, br, xEvent, y);

          if(akeyevt[i].evttype == EventType.KeyPress)
          {
               KeyPressEventArgs kpea = 
                                   (KeyPressEventArgs) akeyevt[i].evtargs;

               string str = String.Format("\x202D{0} (0x{1:X4})", 
                                          kpea.KeyChar, (int) kpea.KeyChar);
               grfx.DrawString(str, Font, br, xChar, y);
          }
          else
          {
               KeyEventArgs kea = (KeyEventArgs) akeyevt[i].evtargs;

               string str = String.Format("{0} ({1})", 
                                          kea.KeyCode, (int) kea.KeyCode);
               grfx.DrawString(str, Font, br, xCode, y);
               grfx.DrawString(kea.Modifiers.ToString(), Font, br, xMods, y);
               grfx.DrawString(kea.KeyData.ToString(), Font, br, xData, y);
               grfx.DrawString(kea.Shift.ToString(), Font, br, xShift, y);
               grfx.DrawString(kea.Control.ToString(), Font, br, xCtrl,  y);
               grfx.DrawString(kea.Alt.ToString(), Font, br, xAlt,   y);
          }
     }                         
}
