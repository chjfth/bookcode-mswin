//---------------------------------------------
// SimpleTreeView.cs © 2001 by Charles Petzold
//---------------------------------------------
using System;
using System.Drawing;
using System.Windows.Forms;

class SimpleTreeView: Form
{
     public static void Main()
     {
          Application.Run(new SimpleTreeView());
     }
     public SimpleTreeView()
     {
          Text = "Simple Tree View";

          TreeView tree = new TreeView();
          tree.Parent = this;
          tree.Dock = DockStyle.Fill;

          tree.Nodes.Add("Animal");
          tree.Nodes[0].Nodes.Add("Dog");
          tree.Nodes[0].Nodes[0].Nodes.Add("Poodle");
          tree.Nodes[0].Nodes[0].Nodes.Add("Irish Setter");
          tree.Nodes[0].Nodes[0].Nodes.Add("German Shepherd");
          tree.Nodes[0].Nodes.Add("Cat");
          tree.Nodes[0].Nodes[1].Nodes.Add("Calico");
          tree.Nodes[0].Nodes[1].Nodes.Add("Siamese");
          tree.Nodes[0].Nodes.Add("Primate");
          tree.Nodes[0].Nodes[2].Nodes.Add("Chimpanzee");
          tree.Nodes[0].Nodes[2].Nodes.Add("Ape");
          tree.Nodes[0].Nodes[2].Nodes.Add("Human");
          tree.Nodes.Add("Mineral");
          tree.Nodes[1].Nodes.Add("Calcium");
          tree.Nodes[1].Nodes.Add("Zinc");
          tree.Nodes[1].Nodes.Add("Iron");
          tree.Nodes.Add("Vegetable");
          tree.Nodes[2].Nodes.Add("Carrot");
          tree.Nodes[2].Nodes.Add("Asparagus");
          tree.Nodes[2].Nodes.Add("Broccoli");
     }
}
