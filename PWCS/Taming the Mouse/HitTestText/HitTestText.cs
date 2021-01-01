//------------------------------------------
// HitTestText.cs (R) 2001 by Charles Petzold
//------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HitTestText: TypeAway
{
     public new static void Main()
     {
          Application.Run(new HitTestText());
     }
     public HitTestText()
     {
          Text += " with Hit-Testing";
          Cursor = Cursors.IBeam;
     }
     protected override void OnMouseDown(MouseEventArgs mea)
     {
          if (strText.Length == 0)
               return;

          Graphics grfx  = CreateGraphics();
          float    xPrev = 0;
          int      i;

          for (i = 0; i < strText.Length; i++)
          {
               SizeF sizef = grfx.MeasureString(strText.Substring(0, i + 1),
                                        Font, Point.Empty, 
                                        StringFormat.GenericTypographic);
 
               if (Math.Abs(mea.X - xPrev) < Math.Abs(mea.X - sizef.Width))
                    break;

               xPrev = sizef.Width;
          }
          iInsert = i;

          grfx.Dispose();
          PositionCaret();
     }
}
          
