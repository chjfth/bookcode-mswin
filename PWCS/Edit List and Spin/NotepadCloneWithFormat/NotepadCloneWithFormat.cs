//-----------------------------------------------------
// NotepadCloneWithFormat.cs © 2001 by Charles Petzold
//-----------------------------------------------------
using Microsoft.Win32;
using System;
using System.Drawing;
using System.Windows.Forms;

class NotepadCloneWithFormat: NotepadCloneWithEdit
{
	// Strings for registry

	const string strWordWrap  = "WordWrap";
	const string strFontFace  = "FontFace";
	const string strFontSize  = "FontSize";
	const string strFontStyle = "FontStyle";
	const string strForeColor = "ForeColor";
	const string strBackColor = "BackColor";
	const string strCustomClr = "CustomColor";

	ColorDialog clrdlg = new ColorDialog();
	MenuItem    miFormatWrap;

	public new static void Main()
	{
		Application.Run(new NotepadCloneWithFormat());
	}
	public NotepadCloneWithFormat()
	{
		strProgName = "Notepad Clone with Format";
		MakeCaption();

		// Format

		MenuItem mi = new MenuItem("F&ormat");
		mi.Popup += new EventHandler(MenuFormatOnPopup);
		Menu.MenuItems.Add(mi);
		int index = Menu.MenuItems.Count - 1;

		// Format Word Wrap

		miFormatWrap = new MenuItem("&Word Wrap");
		miFormatWrap.Click += new EventHandler(MenuFormatWrapOnClick);
		Menu.MenuItems[index].MenuItems.Add(miFormatWrap);

		// Format Font

		mi = new MenuItem("&Font...");
		mi.Click += new EventHandler(MenuFormatFontOnClick);
		Menu.MenuItems[index].MenuItems.Add(mi);

		// Format Background Color

		mi = new MenuItem("Background &Color...");
		mi.Click += new EventHandler(MenuFormatColorOnClick);
		Menu.MenuItems[index].MenuItems.Add(mi);
	}
	protected override void OnLoad(EventArgs ea)
	{
		base.OnLoad(ea);

		// Help

		MenuItem mi = new MenuItem("&Help");
		Menu.MenuItems.Add(mi);
		int index = Menu.MenuItems.Count - 1;

		// Help About

		mi = new MenuItem("&About " + strProgName + "...");
		mi.Click += new EventHandler(MenuHelpAboutOnClick);
		Menu.MenuItems[index].MenuItems.Add(mi);
	}
	void MenuFormatOnPopup(object obj, EventArgs ea)
	{
		miFormatWrap.Checked = txtbox.WordWrap;
	}
	void MenuFormatWrapOnClick(object obj, EventArgs ea)
	{
		MenuItem mi = (MenuItem) obj;
		mi.Checked ^= true;
		txtbox.WordWrap = mi.Checked;
	}
	void MenuFormatFontOnClick(object obj, EventArgs ea)
	{
		FontDialog fontdlg = new FontDialog();

		fontdlg.ShowColor = true;
		fontdlg.Font = txtbox.Font;
		fontdlg.Color = txtbox.ForeColor;

		if (fontdlg.ShowDialog() == DialogResult.OK)
		{
			txtbox.Font = fontdlg.Font;
			txtbox.ForeColor = fontdlg.Color;
		}
	}
	void MenuFormatColorOnClick(object obj, EventArgs ea)
	{
		clrdlg.Color = txtbox.BackColor;

		if (clrdlg.ShowDialog() == DialogResult.OK)
			txtbox.BackColor = clrdlg.Color;
	}
	void MenuHelpAboutOnClick(object obj, EventArgs ea)
	{
		MessageBox.Show(strProgName + " © 2001 by Charles Petzold", 
			strProgName);
	}
	protected override void LoadRegistryInfo(RegistryKey regkey)
	{
		base.LoadRegistryInfo(regkey);

		txtbox.WordWrap = Convert.ToBoolean(
			(int) regkey.GetValue(strWordWrap));
		txtbox.Font = new Font((string) regkey.GetValue(strFontFace),
			float.Parse(
			(string) regkey.GetValue(strFontSize)),
			(FontStyle) regkey.GetValue(strFontStyle));
		txtbox.ForeColor = Color.FromArgb(
			(int) regkey.GetValue(strForeColor));
		txtbox.BackColor = Color.FromArgb(
			(int) regkey.GetValue(strBackColor));
          
		int[] aiColors = new int[16];

		for (int i = 0; i < 16; i++)
			aiColors[i] = (int) regkey.GetValue(strCustomClr + i);

		clrdlg.CustomColors = aiColors;
	}
	protected override void SaveRegistryInfo(RegistryKey regkey)
	{
		base.SaveRegistryInfo(regkey);

		regkey.SetValue(strWordWrap,  Convert.ToInt32(txtbox.WordWrap));
		regkey.SetValue(strFontFace,  txtbox.Font.Name);
		regkey.SetValue(strFontSize,  txtbox.Font.SizeInPoints.ToString());
		regkey.SetValue(strFontStyle, (int) txtbox.Font.Style);
		regkey.SetValue(strForeColor, txtbox.ForeColor.ToArgb());
		regkey.SetValue(strBackColor, txtbox.BackColor.ToArgb());

		for (int i = 0; i < 16; i++)
			regkey.SetValue(strCustomClr + i, clrdlg.CustomColors[i]);
	}
}          
