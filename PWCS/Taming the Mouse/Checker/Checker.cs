//--------------------------------------
// Checker.cs © 2001 by Charles Petzold
//--------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class Checker: Form
{
     protected const int     xNum = 5;       // Number of boxes horizontally
     protected const int     yNum = 4;       // Number of boxes vertically
     protected       bool[,] abChecked = new bool[yNum, xNum];
     protected       int     cxBlock, cyBlock;

     public static void Main()
     {
          Application.Run(new Checker());
     }
     public Checker()
     {
          Text = "Checker";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          ResizeRedraw = true;

		 OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          base.OnResize(ea);            // Or else ResizeRedraw doesn't work

          cxBlock = ClientSize.Width  / xNum;
          cyBlock = ClientSize.Height / yNum;
     }
     protected override void OnMouseUp(MouseEventArgs mea)
     {
          int x = mea.X / cxBlock;
          int y = mea.Y / cyBlock;

          if (x < xNum && y < yNum)
          {
               abChecked[y, x] ^= true;
               Invalidate(new Rectangle(x * cxBlock, y * cyBlock,
                                        cxBlock, cyBlock));
          } 
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          Pen      pen  = new Pen(ForeColor);

          for (int y = 0; y < yNum; y++)
          for (int x = 0; x < xNum; x++)
          {
               grfx.DrawRectangle(pen, x * cxBlock, y * cyBlock, 
                                       cxBlock, cyBlock);
               if (abChecked[y, x])
               {
                    grfx.DrawLine(pen, x      * cxBlock,  y      * cyBlock,
                                      (x + 1) * cxBlock, (y + 1) * cyBlock);
                    grfx.DrawLine(pen, x      * cxBlock, (y + 1) * cyBlock,
                                      (x + 1) * cxBlock,  y      * cyBlock);
               }
          }
     }
 }
