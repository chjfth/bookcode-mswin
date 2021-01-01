//---------------------------------------
// TypeAway.cs (R) 2001 by Charles Petzold
//---------------------------------------
using System;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;
using Petzold.ProgrammingWindowsWithCSharp;

class TypeAway: Form
{
     public static void Main()
     {
          Application.Run(new TypeAway());
     }

     protected Caret  caret;
     protected string strText = "";
     protected int    iInsert = 0;

     public TypeAway()
     {
          Text = "Type Away";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
          
          FontHeight = 24;

          caret = new Caret(this);
          caret.Size = new Size(2, Font.Height);
          caret.Position = new Point(0, 0);
     }
     protected override void OnKeyPress(KeyPressEventArgs kpea)
     {
          caret.Hide();
          Graphics grfx = CreateGraphics();
          grfx.FillRectangle(new SolidBrush(BackColor), 
							 new RectangleF(Point.Empty, 
							 grfx.MeasureString(strText, Font,
							 Point.Empty, StringFormat.GenericTypographic)));

          switch(kpea.KeyChar)
          {
          case '\b':
               if (iInsert > 0)
               {
                    strText = strText.Substring(0, iInsert - 1) +
                                                strText.Substring(iInsert);
                    iInsert -= 1;
               }
               break;

          case '\r':
          case '\n':
               break;

          default:
               if (iInsert == strText.Length)
                    strText += kpea.KeyChar;
               else
                    strText = strText.Substring(0, iInsert) +
                              kpea.KeyChar +
                              strText.Substring(iInsert);
               iInsert++;
               break;
          }
          grfx.TextRenderingHint = TextRenderingHint.AntiAlias;
          grfx.DrawString(strText, Font, new SolidBrush(ForeColor), 
                          0, 0, StringFormat.GenericTypographic);
          grfx.Dispose();

          PositionCaret();
          caret.Show();
     }
     protected override void OnKeyDown(KeyEventArgs kea)
     {
          switch (kea.KeyData)
          {
          case Keys.Left:
               if (iInsert > 0)
                    iInsert--;
               break;

          case Keys.Right:
               if (iInsert < strText.Length)
                    iInsert++;
               break;

          case Keys.Home:
               iInsert = 0;
               break;

          case Keys.End:
               iInsert = strText.Length;
               break;

          case Keys.Delete:
               if (iInsert < strText.Length)
               {
                    iInsert++;
                    OnKeyPress(new KeyPressEventArgs('\b'));
               }
               break;

          default:
               return;
          }
          PositionCaret();
     }
     protected void PositionCaret()
     {
          Graphics grfx = CreateGraphics();
          string str = strText.Substring(0, iInsert);
          StringFormat strfmt = StringFormat.GenericTypographic;
          strfmt.FormatFlags |= StringFormatFlags.MeasureTrailingSpaces;
          SizeF sizef = grfx.MeasureString(str, Font, Point.Empty, strfmt);
          caret.Position = new Point((int)sizef.Width, 0);
          grfx.Dispose();
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics grfx = pea.Graphics;
          grfx.TextRenderingHint = TextRenderingHint.AntiAlias;
          grfx.DrawString(strText, Font, new SolidBrush(ForeColor), 
                          0, 0, StringFormat.GenericTypographic);
     }
}