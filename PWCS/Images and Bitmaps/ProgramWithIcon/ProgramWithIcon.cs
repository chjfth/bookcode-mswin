//-------------------------------------------
// ProgramWithIcon (R) 2001 by Charles Petzold
//-------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class ProgramWithIcon: Form
{
     public static void Main()
     {
          Application.Run(new ProgramWithIcon());
     }
     public ProgramWithIcon()
     {
          Text = "Program with Icon";

          Icon = new Icon(typeof(ProgramWithIcon), 
                          "ProgramWithIcon.ProgramWithIcon.ico");
     }
}