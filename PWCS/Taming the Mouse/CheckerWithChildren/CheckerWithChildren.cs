//--------------------------------------------------
// CheckerWithChildren.cs © 2001 by Charles Petzold
//--------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CheckerWithChildren: Form
{
     protected const int       xNum = 5;
     protected const int       yNum = 4;
     protected CheckerChild[,] acntlChild;

     public static void Main()
     {
          Application.Run(new CheckerWithChildren());
     }
     public CheckerWithChildren()
     {
          Text = "Checker with Children";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          CreateChildren();

		 OnResize(EventArgs.Empty);
     }
     protected virtual void CreateChildren()
     {
          acntlChild = new CheckerChild[yNum, xNum];

          for (int y = 0; y < yNum; y++)
          for (int x = 0; x < xNum; x++)
          {
               acntlChild[y, x] = new CheckerChild();
               acntlChild[y, x].Parent = this;
          }
     }
     protected override void OnResize(EventArgs ea)
     {
          int cxBlock = ClientSize.Width / xNum;
          int cyBlock = ClientSize.Height / yNum;

          for (int y = 0; y < yNum; y++)
          for (int x = 0; x < xNum; x++)
          {
               acntlChild[y, x].Location = new Point(x*cxBlock, y*cyBlock);
               acntlChild[y, x].Size     = new Size(cxBlock, cyBlock);
          }
     }
}
