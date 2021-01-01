//----------------------------------------------
// SysInfoFirstTry.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoFirstTry: Form
{
     public static void Main()
     {
          Application.Run(new SysInfoFirstTry());
     }
     public SysInfoFirstTry()
     {
          Text = "System Information: First Try";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx  = pea.Graphics;
          Brush    brush = new SolidBrush(ForeColor);
          int      y     = 0;

          grfx.DrawString("ArrangeDirection: " +
                          SystemInformation.ArrangeDirection, 
                          Font, brush, 0, y);

          grfx.DrawString("ArrangeStartingPosition: " +
                          SystemInformation.ArrangeStartingPosition, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("BootMode: " +
                          SystemInformation.BootMode, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("Border3DSize: " +
                          SystemInformation.Border3DSize, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("BorderSize: " +
                          SystemInformation.BorderSize, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("CaptionButtonSize: " +
                          SystemInformation.CaptionButtonSize, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("CaptionHeight: " +
                          SystemInformation.CaptionHeight, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("ComputerName: " +
                          SystemInformation.ComputerName, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("CursorSize: " +
                          SystemInformation.CursorSize, 
                          Font, brush, 0, y += Font.Height);

          grfx.DrawString("DbcsEnabled: " +
                          SystemInformation.DbcsEnabled, 
                          Font, brush, 0, y += Font.Height);
    }
}