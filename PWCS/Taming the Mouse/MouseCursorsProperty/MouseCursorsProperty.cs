//---------------------------------------------------
// MouseCursorsProperty.cs (R) 2001 by Charles Petzold
//---------------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class MouseCursorsProperty: Form
{
     Label[] acntl = new Label[28];

     public static void Main()
     {
          Application.Run(new MouseCursorsProperty());
     }
     public MouseCursorsProperty()
     {
          Cursor[] acursor = 
          { 
               Cursors.AppStarting, Cursors.Arrow,       Cursors.Cross,       
               Cursors.Default,     Cursors.Hand,        Cursors.Help,     
               Cursors.HSplit,      Cursors.IBeam,       Cursors.No,          
               Cursors.NoMove2D,    Cursors.NoMoveHoriz, Cursors.NoMoveVert,
               Cursors.PanEast,     Cursors.PanNE,       Cursors.PanNorth,    
               Cursors.PanNW,       Cursors.PanSE,       Cursors.PanSouth,
               Cursors.PanSW,       Cursors.PanWest,     Cursors.SizeAll,     
               Cursors.SizeNESW,    Cursors.SizeNS,      Cursors.SizeNWSE,
               Cursors.SizeWE,      Cursors.UpArrow,     Cursors.VSplit,      
               Cursors.WaitCursor
          };
          string[] astrCursor = 
          { 
               "AppStarting",       "Arrow",             "Cross",       
               "Default",           "Hand",              "Help",     
               "HSplit",            "IBeam",             "No",          
               "NoMove2D",          "NoMoveHoriz",       "NoMoveVert",
               "PanEast",           "PanNE",             "PanNorth",    
               "PanNW",             "PanSE",             "PanSouth",
               "PanSW",             "PanWest",           "SizeAll",     
               "SizeNESW",          "SizeNS",            "SizeNWSE",
               "SizeWE",            "UpArrow",           "VSplit",      
               "WaitCursor" 
          };

          Text = "Mouse Cursors Using Cursor Property";

          for (int i = 0; i < 28; i++)
          {
               acntl[i] = new Label();
               acntl[i].Parent = this;
               acntl[i].Text = astrCursor[i];
               acntl[i].Cursor = acursor[i]; 
               acntl[i].BorderStyle = BorderStyle.FixedSingle;
          }
		 OnResize(EventArgs.Empty);
     }
     protected override void OnResize(EventArgs ea)
     {
          for (int i = 0; i < acntl.Length; i++)
          {
               acntl[i].Bounds = Rectangle.FromLTRB(
                                        (i % 4    ) * ClientSize.Width  / 4, 
                                        (i / 4    ) * ClientSize.Height / 7,
                                        (i % 4 + 1) * ClientSize.Width  / 4, 
                                        (i / 4 + 1) * ClientSize.Height / 7);
          }
     }
}
