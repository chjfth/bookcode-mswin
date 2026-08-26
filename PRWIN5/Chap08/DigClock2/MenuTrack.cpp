#include <CHHI_DEBUG.h>

#include <tchar.h>
#include <assert.h>
#include <windows.h>

#include <vaDbgTs.h>
#include <CHHI_vaDBG_is_vaDbgTs.h>

#include <ospath.h>
#include <fsapi.h>

#include "resource.h"

#include "MenuTrack.h"

#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


void CMenuPop_PlaySound::On_WM_INITMENUPOPUP(
	HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu)
{
	CheckMenuItem(hmenuPopup, ID_PLAYSOUND_NONE,
		!g_is_playsound ? MF_CHECKED : MF_UNCHECKED);

	CheckMenuItem(hmenuPopup, ID_PLAYSOUND_DEFAULT,
		g_is_playsound && g_playsound_filepath.GetValue().is_empty() ? MF_CHECKED : MF_UNCHECKED);

	// Remove old dynamic menuitems
	const int dyn_pos = 2;
	while (DeleteMenu(hmenuPopup, dyn_pos, MF_BYPOSITION));

	// Add dynamic menuitems according to INI[chime_list]list=... strings.
	g_chime_filepaths = SplitToSdrings(g_chime_list.GetValue(), false, _T("\n"), _T(" \t"));
	for (int i = 0; i < g_chime_filepaths.count(); i++)
	{
		const Sdring &filepath = g_chime_filepaths[i];
		const Sdring filenam = ospath::split_filenam(filepath);

		const int cmdid = ID_PLAYSOUND_DYNA_START + i;

		// We only add filenam as menu-item, bcz full filepath could be too long.
		TCHAR menutext[256];
		snTprintf(menutext, _T("(&%d) %s"), i + 1, filenam.c_str());
		AppendMenu(hmenuPopup, MF_STRING, cmdid, menutext);

		BOOL b = SetMenuitem_UserContext(hmenuPopup, cmdid, MenuitemById, (void*)filepath.c_str());
		assert(b);

		if (g_is_playsound)
		{
			if (Sdring::str_match(g_playsound_filepath.GetValue(), filepath))
				CheckMenuItem(hmenuPopup, cmdid, MF_CHECKED);
		}
	}

	// Add final menuitem "Add sound files ..."
	AppendMenu(hmenuPopup, MF_STRING, ID_PLAYSOUND_ADDFILE, _T("&Add sound files ..."));
}

void CMenuPop_PlaySound::On_WM_MENUSELECT(
	HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags)
{
	if (idxItem != 0)
		vaDBG2(_T("Menuitem hover on CMDID: hMenu=0x%X, cmdid=%d"), Ptr2Uint(hmenu), idxItem);
	else if (hSubmenu)
		vaDBG2(_T("Menuitem hover on POPUP: hMenu=0x%X, hSubmenu=0x%X"), Ptr2Uint(hmenu), Ptr2Uint(hSubmenu));
	else
		vaDBG1(_T("Menuitem hover. Weird both zero! flags=0x%X"), flags);

	if (idxItem != m_prev_cmdid)
	{
		vaDBG2(_T("> item(%d) != s_prev_cmdid(%d) , g_chimeplay.PlayStop()"), idxItem, m_prev_cmdid);
		g_chimeplay.PlayStop();
		m_prev_cmdid = idxItem;
	}

	////////

	if (idxItem == ID_PLAYSOUND_DEFAULT)
	{
		g_tooltip.ShowBelowMouse(_T("Preview playing default chime..."));
		g_chimeplay.PlayOnce(ChimePlay::SndPreview, NULL);
	}
	else if (idxItem >= ID_PLAYSOUND_DYNA_START && idxItem < ID_PLAYSOUND_DYNA_END_)
	{
		const TCHAR *filepath = nullptr;
		BOOL b = GetMenuitem_UserContext(hmenu, idxItem, MenuitemById, (void**)&filepath);
		assert(b);
		vaDBG2(_T("Retrieved chime filepath: %s"), filepath);

		Sdring fullpath = GetFullpathRelaToExe(filepath);

		if (fsapi::file_exists(fullpath))
		{
			g_tooltip.ShowBelowMouse(_T("%s\r\n\r\nPreview playing..."), fullpath.c_str());

			auto pserr = g_chimeplay.PlayOnce(ChimePlay::SndPreview, fullpath);
			if (pserr)
			{
				g_tooltip.ShowBelowMouse(_T("%s\r\n\r\nSomething wrong, the system cannot play this sound file."), fullpath.c_str());
			}
		}
		else
		{
			g_tooltip.ShowBelowMouse(
				_T("%s\r\n\r\nThis sound file does NOT exist. Click to remove it from menu."),
				fullpath.c_str());
		}
	}
	else
	{
		g_tooltip.Hide();
		g_chimeplay.PlayStop();
	}

}

#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif
