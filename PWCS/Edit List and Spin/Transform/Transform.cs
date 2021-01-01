//----------------------------------------
// Transform.cs (R) 2001 by Charles Petzold
//----------------------------------------
using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;      // For bitmap 
using System.Windows.Forms;

class Transform: Form
{
     Matrix matrix = new Matrix();
 
     public static void Main()
     {
          Application.Run(new Transform());
     }
     public Transform()
     {
          Text = "Transform";
          ResizeRedraw = true;
          BackColor = Color.White;
          Size += Size;

               // Create modal dialog box.

          MatrixElements dlg = new MatrixElements();
          dlg.Owner    = this;
          dlg.Matrix   = matrix;
          dlg.Changed += new EventHandler(MatrixDialogOnChanged);
          dlg.Show();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          DrawAxes(grfx);
          grfx.Transform = matrix;
          DrawHouse(grfx);
     }
      void DrawAxes(Graphics grfx)
     {
          Brush        brush  = Brushes.Black;
          Pen          pen    = Pens.Black;
          StringFormat strfmt = new StringFormat();

               // Horizontal axis

          strfmt.Alignment = StringAlignment.Center;

          for (int i = 1; i <= 10; i++)
          {
               grfx.DrawLine(pen, 100 * i, 0, 100 * i, 10);
               grfx.DrawString((i * 100).ToString(), Font, brush, 
                               100 * i, 10, strfmt);
               grfx.DrawLine(pen, 100 * i, 10 + Font.Height, 
                                  100 * i, ClientSize.Height);
          }

               // Vertical axis

          strfmt.Alignment     = StringAlignment.Near;
          strfmt.LineAlignment = StringAlignment.Center;

          for (int i = 1; i <= 10; i++)
          {
               grfx.DrawLine(pen, 0, 100 * i, 10, 100 * i);
               grfx.DrawString((i * 100).ToString(), Font, brush, 
                               10, 100 * i, strfmt);
               float cxText = grfx.MeasureString(
                                        (i * 100).ToString(), Font).Width;
               grfx.DrawLine(pen, 10 + cxText,      100 * i, 
                                  ClientSize.Width, 100 * i);
          }
     }
     void DrawHouse(Graphics grfx)
     {
          Rectangle   rectFacade  =   new Rectangle( 0, 40, 100,  60);
          Rectangle   rectDoor    =   new Rectangle(10, 50,  25,  50);
          Rectangle[] rectWindows = { new Rectangle(50, 50,  10,  10),
                                      new Rectangle(60, 50,  10,  10),
                                      new Rectangle(70, 50,  10,  10),
                                      new Rectangle(50, 60,  10,  10),
                                      new Rectangle(60, 60,  10,  10),
                                      new Rectangle(70, 60,  10,  10),
                                      new Rectangle(15, 60,   5,   7),
                                      new Rectangle(20, 60,   5,   7),
                                      new Rectangle(25, 60,   5,   7) };
          Rectangle   rectChimney =   new Rectangle(80,  5,  10,  35);
          Point[]     ptRoof      = { new Point( 50,  0), 
                                      new Point(  0, 40), 
                                      new Point(100, 40) };

               // Create bitmap and brush for chimney.

          Bitmap bitmap = new Bitmap(8, 6); 

          byte[] bits = { 0, 0, 0, 0, 0, 0, 0, 0,
                          1, 1, 1, 0, 1, 1, 1, 0, 
                          1, 1, 1, 0, 1, 1, 1, 0, 
                          0, 0, 0, 0, 0, 0, 0, 0, 
                          1, 0, 1, 1, 1, 0, 1, 1,
                          1, 0, 1, 1, 1, 0, 1, 1,

                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 1, 1, 1, 1, 1, 1 };

          for (int i = 0; i < 48; i++)
               bitmap.SetPixel(i % 8, i / 8, 
                         bits[i] == 1 ? Color.DarkGray: Color.LightGray);

          Brush brush = new TextureBrush(bitmap);

               // Draw entire house.

          grfx.FillRectangle(Brushes.LightGray, rectFacade);
          grfx.DrawRectangle(Pens.Black,        rectFacade);

          grfx.FillRectangle(Brushes.DarkGray, rectDoor);
          grfx.DrawRectangle(Pens.Black,       rectDoor);

          grfx.FillRectangles(Brushes.White, rectWindows);
          grfx.DrawRectangles(Pens.Black,    rectWindows);

          grfx.FillRectangle(brush,      rectChimney);
          grfx.DrawRectangle(Pens.Black, rectChimney);

          grfx.FillPolygon(Brushes.DarkGray, ptRoof);
          grfx.DrawPolygon(Pens.Black,       ptRoof);
     }
     void MatrixDialogOnChanged(object obj, EventArgs ea)
     {
          MatrixElements dlg = (MatrixElements) obj;
          matrix = dlg.Matrix;
          Invalidate();
     }
}