//--------------------------------------
// HexCalc.cs (R) 2001 by Charles Petzold
//--------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class HexCalc: Form
{
     Button btnResult;
     ulong  ulNum       = 0;
     ulong  ulFirstNum  = 0;     
     bool   bNewNumber  = true;
     char   chOperation = '=';

     public static void Main()
     {
          Application.Run(new HexCalc());
     }
     public HexCalc()
     {
          Text = "Hex Calc";
          Icon = new Icon(GetType(), "HexCalc.HexCalc.ico");
          FormBorderStyle = FormBorderStyle.FixedSingle;
          MaximizeBox = false;

          new CalcButton(this, "D",       'D',  8,  24, 14, 14);
          new CalcButton(this, "A",       'A',  8,  40, 14, 14);
          new CalcButton(this, "7",       '7',  8,  56, 14, 14);
          new CalcButton(this, "4",       '4',  8,  72, 14, 14);
          new CalcButton(this, "1",       '1',  8,  88, 14, 14);
          new CalcButton(this, "0",       '0',  8, 104, 14, 14);
          new CalcButton(this, "E",       'E', 26,  24, 14, 14);
          new CalcButton(this, "B",       'B', 26,  40, 14, 14);
          new CalcButton(this, "8",       '8', 26,  56, 14, 14);
          new CalcButton(this, "5",       '5', 26,  72, 14, 14);
          new CalcButton(this, "2",       '2', 26,  88, 14, 14);
          new CalcButton(this, "Back", '\x08', 26, 104, 32, 14);
          new CalcButton(this, "C",       'C', 44,  40, 14, 14);
          new CalcButton(this, "F",       'F', 44,  24, 14, 14);
          new CalcButton(this, "9",       '9', 44,  56, 14, 14);
          new CalcButton(this, "6",       '6', 44,  72, 14, 14);
          new CalcButton(this, "3",       '3', 44,  88, 14, 14);
          new CalcButton(this, "+",       '+', 62,  24, 14, 14);
          new CalcButton(this, "-",       '-', 62,  40, 14, 14);
          new CalcButton(this, "*",       '*', 62,  56, 14, 14);
          new CalcButton(this, "/",       '/', 62,  72, 14, 14);
          new CalcButton(this, "%",       '%', 62,  88, 14, 14);
          new CalcButton(this, "Equals",  '=', 62, 104, 32, 14);
          new CalcButton(this, "&&",      '&', 80,  24, 14, 14);
          new CalcButton(this, "|",       '|', 80,  40, 14, 14);
          new CalcButton(this, "^",       '^', 80,  56, 14, 14);
          new CalcButton(this, "<",       '<', 80,  72, 14, 14);
          new CalcButton(this, ">",       '>', 80,  88, 14, 14);

          btnResult = 
               new CalcButton(this, "0", '\x1B', 8, 4, 86, 14);

          foreach (Button btn in Controls)
               btn.Click += new EventHandler(ButtonOnClick);

          ClientSize = new Size(102, 126);
          Scale(Font.Height / 8f);
     }
     protected override void OnKeyPress(KeyPressEventArgs kpea)
     {
          char chKey = Char.ToUpper(kpea.KeyChar);

          if (chKey == '\x0D')
               chKey = '=';

          for (int i = 0; i < Controls.Count; i++)
          {
               CalcButton btn = (CalcButton) Controls[i];

               if (chKey == btn.chKey)
               {
                    InvokeOnClick(btn, EventArgs.Empty);
                    break;
               }
          }
     }
     void ButtonOnClick(object obj, EventArgs ea)
     {
          CalcButton btn = (CalcButton) obj;

          if (btn.chKey == '\x08')
               ulNum /= 16;

          else if (btn.chKey == '\x1B')
               ulNum = 0;

          else if (Char.IsLetterOrDigit(btn.chKey))    // Hex digit
          {
               if (bNewNumber)
               {
                    ulFirstNum = ulNum;
                    ulNum = 0;
                    bNewNumber = false;
               }
          
               if (ulNum <= ulong.MaxValue >> 4)
                    ulNum = 16 * ulNum + 
                         (ulong)(btn.chKey -
                              (Char.IsDigit(btn.chKey) ? '0' : 'A' - 10));
          }
          else                                         // Operation
          {
               if(!bNewNumber)
               {
                    switch(chOperation)
                    {
                    case '=': ulNum = ulNum; break;
                    case '+': ulNum = ulFirstNum + ulNum; break;
                    case '-': ulNum = ulFirstNum - ulNum; break;
                    case '*': ulNum = ulFirstNum * ulNum; break;
                    case '&': ulNum = ulFirstNum & ulNum; break;
                    case '|': ulNum = ulFirstNum | ulNum; break;
                    case '^': ulNum = ulFirstNum ^ ulNum; break;
                    case '<': ulNum = ulFirstNum << (int)ulNum; break;
                    case '>': ulNum = ulFirstNum >> (int)ulNum; break;
                    case '/': ulNum = ulNum != 0 ? 
                                        ulFirstNum / ulNum : ulong.MaxValue; 
                              break;
                    case '%': ulNum = ulNum != 0 ? 
                                        ulFirstNum % ulNum : ulong.MaxValue; 
                              break;
                    default:  ulNum = 0; break;
                    }
               }
               bNewNumber = true;
               chOperation = btn.chKey;
          }
          btnResult.Text = String.Format("{0:X}", ulNum);
     }
}
class CalcButton: Button
{
     public char chKey;

     public CalcButton(Control parent, string str, char chkey, 
                       int x, int y, int cx, int cy)
     {
          Parent   = parent;
          Text     = str;
          chKey    = chkey;
          Location = new Point(x, y);
          Size     = new Size(cx, cy);
		  SetStyle(ControlStyles.Selectable, false);
     }
}