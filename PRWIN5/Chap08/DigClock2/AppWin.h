#pragma once

#include <mswin/WM_MOUSELEAVE_helper.h>
#include <mswin/Tooltip-helper.h>

#include "CxxRawWinBase.h"
#include "utils.h"

class CAppWin : public CxxRawWinBase
{
public:
	CAppWin() { _ctor(); }
	void _ctor();

public: // Override base-class's default actions

	virtual BOOL RegisterAndCreate(const TCHAR *szAppName);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

//	void StopShaking() { m_winshaker.ShakeStop(); }

	Sdring GetCountDownCfg() {
		return m_seconds_countdown_cfg.GetValueAsString(); // Returns sth like "00:01:00"
	}

	bool StartCountDown(const TCHAR *cfghms);

public: // Window message handling //	todo : change to protected

	BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT);
	void Cls_OnPaint(HWND hwnd);
	void Cls_OnDestroy(HWND hwnd);

	void Cls_OnCommand(HWND hwnd, int cmdid, HWND hwndCtl, UINT codeNotify);

	void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy);
	void Cls_OnMove(HWND hwnd, int x, int y);
	void Cls_OnTimer(HWND hwnd, UINT id);

	void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

	void Cls_OnInitMenuPopup(HWND hwnd, HMENU hmenuPopup, UINT item, BOOL fSystemMenu);
	void Cls_OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hSubmenu, UINT flags);

	void Cls_OnDropFiles(HWND hwnd, HDROP hdrop);


private:
	void OnWinMoveSize(HWND hwnd);
	void My_OnMouseLeave(HWND hwnd);

	void DoTimer(HWND hwnd, int idtimer);
	
	void RefreshTheClock(HDC hdc);
	void RefreshDateBar(HDC hdc);
	void DisplayCountDown(HDC hdc);

	void do_CountdownDone(HWND hwnd); // Time due

	void winenv_ReloadSetting(HWND hwnd);
	void ReloadIni_and_Redraw(HWND hwnd);

	void my_AdjustClientRect(HWND hwnd, bool is_default_width=false);
	void my_MoveWindow_byClientRect(HWND hwnd, const RECT& rcClient);

	int clock_cy_from_cx(int cx);

	bool ClientRectFromINI(HWND hwnd, RECT *prect);

	void AddNewFiles_to_ChimeList_SingleDir(HWND hwnd, const Sdrings &ss_input);
	void AddNewFiles_to_ChimeList(HWND hwnd, const Sdrings &ss_input);

	Sdrings chime_list_GetValue();
	void chime_list_SetValue(const Sdrings& chime_filepaths);

private: // private program states here.

	bool m_firstUpdateWindowDone = false;

	// Clock representation related:
	BOOL m_f24Hour = FALSE;
	BOOL m_fSuppressHighDigit = FALSE;

	// Countdown related:
	int m_seconds_remain = 0;
	DWORD m_msectick_start = 0; // value from GetTickCount()

	// Windows position related:
	int m_axClient = 0, m_ayClient = 0; // Clock window client-area absolute(screen) position(left,top).
	int m_cxClient = 0, m_cyClient = 0; // Clock window client-area size pixels

	int m_iso_client_cx = 0; // maybe less than s_cxClient, if you squeeze clock-window's height.

	// Mouse control related:
	POINT m_pos_mousedown; // client-area position
	bool m_is_dragging = false;
	bool m_is_moved = false;

	CWmMouseleaveHelper m_mouselvp;

	// Other:

	HMENU m_hmenuRootPopup = NULL;
	CMenuTracker m_menu_tracker;

	WindowShaker m_winshaker;

	CTooltipSimple m_tooltip;

	static UINT s_msgval_playsound_done; // dynamic window-message value

private: // friends
	
	// These need to access DataXIni related members
	friend class CMenuPop_Root;
	friend class CMenuPop_ShowDate;
	friend class CMenuPop_ShakeWindow;
	friend class CMenuPop_PlaySound;

	// This needs to access .m_shake_window_repaint
	friend class WindowShaker;

private: // INI cfg related privates

	DataXString_AutoSaveIni<ClockMode_et> m_ClockMode;
	DataXString_AutoSaveIni<bool> m_isShowDate;
	DataXString_AutoSaveIni<bool> m_isShowTimezone;

	DataXString_AutoSaveIni<bool> m_is_always_on_top;
	DataXString_AutoSaveIni<bool> m_is_change_color;
	DataXString_AutoSaveIni<bool> m_is_show_title;

	DataXString_AutoSaveIni<int, Format_int_as_HHMMSS> m_seconds_countdown_cfg;
	DataXString_AutoSaveIni<int> m_timedue_shake_seconds;
	DataXString_AutoSaveIni<bool> m_shake_window_repaint; // not on UI yet, only from INI

	DataXString_AutoSaveIni<bool> m_is_playsound;
	DataXString_AutoSaveIni<Sdring> m_playsound_filepath;
	DataXString_AutoSaveIni<Sdring> m_chime_list;

	DataXString_BindIni<Uint, Format_COLORREF_as_RGB> m_dxDigitColor;
	DataXString_BindIni<RECT> m_dxClientRect;

};

