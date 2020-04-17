//---------------------------------------------
// SysInfoColumns.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoColumns: Form
{
     public static void Main()
     {
          Application.Run(new SysInfoColumns());
     }
     public SysInfoColumns()
     {
          Text = "System Information: Columns";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx  = pea.Graphics;
          Brush    brush = new SolidBrush(ForeColor);
          SizeF    sizef;
          float    cxCol, y = 0;
          int      cySpace;

          sizef   = grfx.MeasureString("ArrangeStartingPosition ", Font);
          cxCol   = sizef.Width;
          cySpace = Font.Height;

          grfx.DrawString("ArrangeDirection", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.ArrangeDirection.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("ArrangeStartingPosition", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.ArrangeStartingPosition.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("BootMode", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.BootMode.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("Border3DSize", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.Border3DSize.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("BorderSize", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.BorderSize.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("CaptionButtonSize", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.CaptionButtonSize.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("CaptionHeight", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.CaptionHeight.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("ComputerName", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.ComputerName, 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("CursorSize", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.CursorSize.ToString(), 
                          Font, brush, cxCol, y);
          y += cySpace;

          grfx.DrawString("DbcsEnabled", Font, brush, 0, y);
          grfx.DrawString(SystemInformation.DbcsEnabled.ToString(), 
                          Font, brush, cxCol, y);
    }
}
