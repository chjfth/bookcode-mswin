#pragma once

#include <tchar.h>
#include <windows.h>
#include <mswin/MenuTracker.h>
#include <mswin/utils_wingui.h>

#include "utils.h"


class CMenuPop_Root : public IMenuPop
{
public:
	CMenuPop_Root() {}

	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) override
	{
		CheckMenuItem(hmenuPopup, IDM_COUNTDOWN_MODE,
			g_ClockMode == CM_Countdown ? MF_CHECKED : MF_UNCHECKED);

		EnableMenuItem(hmenuPopup, IDM_STOP_COUNTDOWN,
			g_seconds_remain > 0 ? MF_ENABLED : MF_GRAYED);

		CheckMenuItem(hmenuPopup, IDM_ALWAYS_ON_TOP,
			s_is_always_on_top ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_CLICK_CHANGE_COLOR,
			s_is_change_color ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOW_TITLE,
			s_is_show_title ? MF_CHECKED : MF_UNCHECKED);
	}
};

//////////////////////////////////////////////////////////////////////////

class CMenuPop_ShowDate : public IMenuPop
{
public:
	CMenuPop_ShowDate() {}

	virtual void On_Init(HMENU hmenuPopup, const TCHAR *popname) override
	{
		// Use bullet-mark instead of check-mark, bcz the 3 options are mutual exclusive.

		Menuitem_Tune_MFTxxx(hmenuPopup, IDM_SHOWDATE_NO, MenuitemById, MFT_RADIOCHECK, 0);
		Menuitem_Tune_MFTxxx(hmenuPopup, IDM_SHOWDATE_YES, MenuitemById, MFT_RADIOCHECK, 0);
		Menuitem_Tune_MFTxxx(hmenuPopup, IDM_SHOWDATE_TIMEZONE, MenuitemById, MFT_RADIOCHECK, 0);
	}

	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) override
	{
		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_NO,
			g_isShowDate ? MF_UNCHECKED : MF_CHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_YES,
			(g_isShowDate && !g_isShowTimezone) ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_TIMEZONE,
			(g_isShowDate && g_isShowTimezone) ? MF_CHECKED : MF_UNCHECKED);

	}
};

//////////////////////////////////////////////////////////////////////////

class CMenuPop_ShakeWindow : public IMenuPop
{
public:
	CMenuPop_ShakeWindow() {}

	virtual void On_Init(HMENU hmenuPopup, const TCHAR *popname) override
	{
	}

	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) override
	{
		bool is_stock = false;
		const int shake_seconds = g_timedue_shake_seconds;

		CheckMenuItem(hmenuPopup, ID_SHAKE_NOSHAKE,
			shake_seconds == 0 ? (is_stock = true, MF_CHECKED) : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, ID_SHAKE_1SEC,
			shake_seconds == 1 ? (is_stock = true, MF_CHECKED) : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, ID_SHAKE_3SEC,
			shake_seconds == 3 ? (is_stock = true, MF_CHECKED) : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, ID_SHAKE_5SEC,
			shake_seconds == 5 ? (is_stock = true, MF_CHECKED) : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, ID_SHAKE_FOREVER,
			shake_seconds < 0 ? (is_stock = true, MF_CHECKED) : MF_UNCHECKED);

		bool is_custom = IsMenuitemExist_byID(hmenuPopup, ID_SHAKE_CUSTOM_SEC);

		if (is_stock)
		{
			if (is_custom)
			{
				DeleteMenu(hmenuPopup, ID_SHAKE_CUSTOM_SEC, MF_BYCOMMAND);
			}
		}
		else
		{
			// User set in INI a seconds value that is not "stock".
			// So we add an extra menu item to exhibit the custom value from INI.
			if (!is_custom)
			{
				AppendMenu(hmenuPopup, MF_STRING, ID_SHAKE_CUSTOM_SEC, _T("set-soon"));
			}
			TCHAR text[40];
			snTprintf(text, _T("%d seconds (from INI)"), shake_seconds);
			SetMenuitemText_byID(hmenuPopup, ID_SHAKE_CUSTOM_SEC, text);
			CheckMenuItem(hmenuPopup, ID_SHAKE_CUSTOM_SEC, MF_CHECKED);
		}
	}
};

//////////////////////////////////////////////////////////////////////////

class CMenuPop_PlaySound : public IMenuPop
{
public:
	CMenuPop_PlaySound() {}

	virtual void On_WM_INITMENUPOPUP(
		HWND hwnd, HMENU hmenuPopup, UINT uItem, BOOL isSystemMenu) override;

	virtual void On_WM_MENUSELECT(
		HWND hwnd, HMENU hmenu, int idxItem, HMENU hSubmenu, UINT flags) override;

private:
	int m_prev_cmdid = 0;
};
