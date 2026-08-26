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

