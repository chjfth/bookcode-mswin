//----------------------------------------------
// SysInfoListView.cs (R) 2001 by Charles Petzold
//----------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SysInfoListView: Form
{
     public static void Main()
     {
          Application.Run(new SysInfoListView());
     }
     public SysInfoListView()
     {
          Text = "System Information (List View)";

               // Create ListView control.

          ListView listview = new ListView();
          listview.Parent = this;
          listview.Dock = DockStyle.Fill;
          listview.View = View.Details;
          
               // Define columns based on maximum string widths.

          Graphics grfx = CreateGraphics();

          listview.Columns.Add("Property", 
                    (int) SysInfoReflectionStrings.MaxLabelWidth(grfx, Font),
                    HorizontalAlignment.Left);

          listview.Columns.Add("Value",
                    (int) SysInfoReflectionStrings.MaxValueWidth(grfx, Font),
                    HorizontalAlignment.Left);

          grfx.Dispose();

               // Get the data that will be displayed.

          int      iNumItems  = SysInfoReflectionStrings.Count;
          string[] astrLabels = SysInfoReflectionStrings.Labels;
          string[] astrValues = SysInfoReflectionStrings.Values;

               // Define the items and subitems.

          for (int i = 0; i < iNumItems; i++)
          {
               ListViewItem lvi = new ListViewItem(astrLabels[i]);
               lvi.SubItems.Add(astrValues[i]);
               listview.Items.Add(lvi);
          }
     }
}        
