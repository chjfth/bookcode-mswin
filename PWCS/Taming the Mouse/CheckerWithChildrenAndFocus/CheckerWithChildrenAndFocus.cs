//----------------------------------------------------------
// CheckerWithChildrenAndFocus.cs (R) 2001 by Charles Petzold
//----------------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class CheckerWithChildrenAndFocus: CheckerWithChildren
{
     public new static void Main()
     {
          Application.Run(new CheckerWithChildrenAndFocus());
     }
     public CheckerWithChildrenAndFocus()
     {
          Text = "Checker with Children and Focus";
     }
     protected override void CreateChildren()
     {
          acntlChild = new CheckerChildWithFocus[yNum, xNum];

          for (int y = 0; y < yNum; y++)
          for (int x = 0; x < xNum; x++)
          {
               acntlChild[y, x] = new CheckerChildWithFocus();
               acntlChild[y, x].Parent = this;
          }
     }
}
