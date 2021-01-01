//-------------------------------------------
// HelloPrinter.cs (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Drawing.Printing;
using System.Windows.Forms;

class HelloPrinter: Form
{
     public static void Main()
     {
          Application.Run(new HelloPrinter());
     }
     public HelloPrinter()
     {
          Text = "Hello Printer!";
          BackColor = SystemColors.Window;
          ForeColor = SystemColors.WindowText;
     }
     protected override void OnPaint(PaintEventArgs pea)
     {
          Graphics     grfx   = pea.Graphics;
          StringFormat strfmt = new StringFormat();

          strfmt.Alignment = strfmt.LineAlignment = StringAlignment.Center;

          grfx.DrawString("Click to print", Font, new SolidBrush(ForeColor),
                          ClientRectangle, strfmt);
     }
     protected override void OnClick(EventArgs ea)
     {
          PrintDocument prndoc = new PrintDocument();

          prndoc.DocumentName = Text;
          prndoc.PrintPage +=
			     new PrintPageEventHandler(PrintDocumentOnPrintPage);
          prndoc.Print();
     }
     void PrintDocumentOnPrintPage(object obj, PrintPageEventArgs ppea)
     {
          Graphics grfx = ppea.Graphics;

          grfx.DrawString(Text, Font, Brushes.Black, 0, 0);

          SizeF sizef = grfx.MeasureString(Text, Font);

          grfx.DrawLine(Pens.Black, sizef.ToPointF(), 
                                    grfx.VisibleClipBounds.Size.ToPointF());
     }
}
