//---------------------------------------
// BlockOut.cs © 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class BlockOut: Form
{
     bool      bBlocking, bValidBox;
     Point     ptBeg, ptEnd;
     Rectangle rectBox;

     public static void Main()
     {
          Application.Run(new BlockOut());
     }
     public BlockOut()
     {
          Text = "Blockout Rectangle with Mouse";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (mea.Button == MouseButtons.Left)
          {
               ptBeg = ptEnd = new Point(mea.X, mea.Y);

               Graphics grfx = CreateGraphics();
               grfx.DrawRectangle(new Pen(ForeColor), Rect(ptBeg, ptEnd));
               grfx.Dispose();

               bBlocking = true;
          }
     }
     protected override void OnMouseMove(MouseEventArgs mea)
     {
          if (bBlocking)
          {
               Graphics grfx = CreateGraphics();
               grfx.DrawRectangle(new Pen(BackColor), Rect(ptBeg, ptEnd));
               ptEnd = new Point(mea.X, mea.Y);
               grfx.DrawRectangle(new Pen(ForeColor), Rect(ptBeg, ptEnd));
               grfx.Dispose();
               Invalidate();
          }
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          if (bBlocking && mea.Button == MouseButtons.Left)
          {
               Graphics grfx = CreateGraphics();
               rectBox = Rect(ptBeg, new Point(mea.X, mea.Y));
               grfx.DrawRectangle(new Pen(ForeColor), rectBox);
               grfx.Dispose();

               bBlocking = false;
               bValidBox = true;
               Invalidate();
          }
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;

          if (bValidBox)
               grfx.FillRectangle(new SolidBrush(ForeColor), rectBox);

          if (bBlocking)
               grfx.DrawRectangle(new Pen(ForeColor), Rect(ptBeg, ptEnd));
     }
     Rectangle Rect(Point ptBeg, Point ptEnd)
     {
          return new Rectangle(Math.Min(ptBeg.X, ptEnd.X),
                               Math.Min(ptBeg.Y, ptEnd.Y),
                               Math.Abs(ptEnd.X - ptBeg.X),
                               Math.Abs(ptEnd.Y - ptBeg.Y));
     }
}
