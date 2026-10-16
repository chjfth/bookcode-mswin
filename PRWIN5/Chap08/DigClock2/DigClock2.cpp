/*-----------------------------------------
DIGCLOCK.c -- Digital Clock(c) Charles Petzold, 1998
DigClock2.cpp -- Updated by Jimm Chen

Since 2022.05:
* Thin window border.
* Drag window by clicking anywhere inside the window.
* Move window by pressing arrow keys, Ctrl to accelerate.
* Right-click context menu, toggle always-on-top.
* Select different color by mouse clicking.
* Scale down "second" value display(75%), so to have it stand out.
* Toggle showing window title, with EXE file name as title text.
* Eliminate UI flickering by MemDC-double-buffering.

Since 2024.10:
* Add Countdown mode.

Since 2026.03: (v1.8)
* Optionally show Date at bottom bar.

Since 2026.05: (v2.2)
* User options are saved to DigClock2.ini, including last window pos.
  INI along-side EXE is preferred; if saving fail, fallback to $HOME dir.
* Add "Reset all settings"(load program default) menu, or hotkey Ctrl+Alt+D .
* Developer hotkey: Ctrl+Alt+L, reload INI settings.
* Now user can manually set arbitrary color from INI, like this:
*    DigitColor=RGB(255,168,88)

Since 2026.08: (v2.5)
* Provides intensive count-down due UI feedback. 
  > User can customize window shaking duration(seconds).
  > User can customize a wav/mp3 sound file to play.

Since 2026.10: (v2.7)
* Manage HWNDs in C++ class, eliminate most global vars.
* Now WinMain() can detect C++ memory leak on program quit.

-----------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <CHHI_DEBUG.h>

#include <tchar.h>
#include <assert.h>
#include <windows.h>
#include <ShellAPI.h>
#include <windowsx.h> // should be after ShellAPI.h to see _INC_SHELLAPI
#include <CommCtrl.h>
#include <shlwapi.h>
#include <ShellAPI.h>
#include <WinUser.h> // WM_DROPFILES
#include <ShlObj-winxp-patch.h>
#include "resource.h"

#include <vaDbgTs.h>
//#include <vaDbgTs_util.h>
#include <CHHI_vaDBG_is_vaDbgTs.h>

#include <commdefs.h>
#include <CHwndTimer.h>
#include <RECTxy.h>
#include <win32cozy.h>
#include <fsapi.h>
#include <ospath.h>
	using namespace ospath;
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/util_commdlg.h>
#include <WinMultiMon.h>

#include <snTprintf.h>

#include <InterpretConst.h>
	using namespace itc;
#include <DataXString.h>
#include <DataXIni.h>

#include <mswin/IPlaySound_mswin.h>
#include <mswin/MenuTracker.h>

#include "iversion.h"
#include "utils.h"
#include "MenuTrack.h"

#include "AppWin.h"
#include "DlgCountdownCfg.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


#define MY_TIMER_INTERVAL_1000ms 1000 // request exactly 1 seconds per WM_TIMER callback

#define ID_TIMER_SECONDS_TICK   1
#define ID_TIMER_HIDE_CFG_PANEL 2 // timer to hide countdown cfg panel

#define LESS_1millisec 1

#define LOGICX_HHMMSS 276
#define LOGICY_HHMMSS 72
#define LOGICY_DATE 24  // since v1.8, show a date-bar at bottom

#define DELAY_SAVE_INI_MILLISEC 1000

HINSTANCE g_hInstance;

#define INI_SECNAME _T("cfg")
#define INI_SECNAME_LISTS _T("file_lists")

const int g_init_client_cx = 188; // Default main-window client-area size(96dpi pixels)

const int SHAKE_TIMER_INTERVAL = 25; // millisec
const int SHAKE_NUDGE_MAX = 20; // for 96dpi monitor

const TCHAR * const g_audio_extnames[] = { _T(".wav"), _T(".mp3") };
const int n_audio_extnames = ARRAYSIZE(g_audio_extnames);

UINT CAppWin::s_msgval_playsound_done = 0;


void InitOnce()
{
	g_hInstance = GetModuleHandle(NULL);
	
	InitCommonControls();
	// -- WinXP requires this, otherwise, g_hdlgCountdownCfg will be NULL.
}

int in_WinMain(int argc, const TCHAR *argv[])
{
	InitOnce();
	HINSTANCE hInstance = g_hInstance;

	static TCHAR szAppName[] = TEXT(APPNAME);
	
	DataXIni xini;
	gp.xini = &xini;

	ChimePlay chimeplay;
	gp.chimeplay = &chimeplay;

	CAppWin appwin;
	gp.appwin = &appwin;

	BOOL succ = appwin.RegisterAndCreate(szAppName);
	assert(succ);
	HWND hwndApp = appwin.Hwnd();
	assert(IsWindow(hwndApp));

	CDlgCountdownCfg dlgFloatbar;
	HWND hdlgFloatbar = dlgFloatbar.CreateModeless(hInstance, MAKEINTRESOURCE(IDD_COUNTDOWN_CFG), hwndApp);
	assert(IsWindow(hdlgFloatbar));
	gp.hdlgFloatbar = hdlgFloatbar;

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	assert(hAccel);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwndApp, hAccel, &msg))
		{
			if (!IsDialogMessage(hdlgFloatbar, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return (int)msg.wParam;
}

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int iCmdShow)
{
	int argc = __argc;
#ifdef UNICODE
	PCTSTR* argv = (PCTSTR*)CommandLineToArgvW(GetCommandLine(), &argc);
#else
	PCTSTR* argv = (PCTSTR*)__argv;
#endif
	// Use argc and argv[] below, just like what traditional main() does.

	// _CrtSetBreakAlloc(205);
	MSVCRT_MemCheckStart(foo);

	int ret = in_WinMain(argc, argv);

	bool isleak = MSVCRT_MemCheckEnd_IsLeak(foo);
	if (isleak) {
		OutputDebugString(_T("Suspect memleak in my C++ code.\n"));
		return 4;
	}
	else {
		OutputDebugString(_T("No memleak.\n"));
		return ret;
	}
}


#define MK_DATA_AutoSaveINI(datatype, varname, keyname, default_val) \
	varname.~DataXString_AutoSaveIni(); \
	new(&varname) DataXString_AutoSaveIni<datatype>(*gp.xini, INI_SECNAME, _T(keyname), default_val);

#define MY_DATA_AutoSaveINI_FORMAT(datatype, varname, format, keyname, default_val) \
	varname.~DataXString_AutoSaveIni(); \
	new(&varname) DataXString_AutoSaveIni<datatype, format>(*gp.xini, INI_SECNAME, _T(keyname), default_val);

#define MK_DATA_AutoSaveINI_secname(datatype, varname, secname, keyname, default_val) \
	varname.~DataXString_AutoSaveIni(); \
	new(&varname) DataXString_AutoSaveIni<datatype>(*gp.xini, secname, _T(keyname), default_val);


void CAppWin::_ctor()
{
	m_pos_mousedown = POINT();

	//////////////////////////////////////////////////////////////////////////

	MK_DATA_AutoSaveINI(ClockMode_et, m_ClockMode, "ClockMode", CM_WallTime);

	MK_DATA_AutoSaveINI(bool, m_isShowDate, "IsShowDate", false);
	MK_DATA_AutoSaveINI(bool, m_isShowTimezone, "IsShowTimezone", false);
	MK_DATA_AutoSaveINI(bool, m_is_always_on_top, "AlwaysOnTop", true);
	MK_DATA_AutoSaveINI(bool, m_is_change_color, "IsClickToChangeColor", false);
	MK_DATA_AutoSaveINI(bool, m_is_show_title, "IsShowWindowTitle", false);

	MY_DATA_AutoSaveINI_FORMAT(int, m_seconds_countdown_cfg, Format_int_as_HHMMSS, "CountdownCfg", _T("00:01:00"));

	MK_DATA_AutoSaveINI(int, m_timedue_shake_seconds, "TimeDueShakeSeconds", 1);
	MK_DATA_AutoSaveINI(bool, m_shake_window_repaint, "ShakeWindowRepaint", true);

	MK_DATA_AutoSaveINI(bool, m_is_playsound, "PlaySoundOnTimeDue", true);
	MK_DATA_AutoSaveINI(Sdring, m_playsound_filepath, "PlaySoundFilepath", _T(""));

	MK_DATA_AutoSaveINI_secname(Sdring, m_chime_list, INI_SECNAME_LISTS, "chime_list", _T(""));
	// -- chime(.wav/.mp3) filepaths separated by \n.

	// The following two: I do NOT apply auto-save on them.

	new(&m_dxClientRect) DataXString_BindIni<RECT>(
		*gp.xini, INI_SECNAME, _T("ClientAreaRect"));

	new(&m_dxDigitColor) DataXString_BindIni<Uint, Format_COLORREF_as_RGB>(
		*gp.xini, INI_SECNAME, _T("DigitColor"), _T("RGB(64,160,255)")); // default is sky blue
}


BOOL CAppWin::RegisterAndCreate(const TCHAR *szAppName)
{
	HINSTANCE hInstance = g_hInstance;

	WNDCLASS wndclass = {};
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = CxxRawWinBase::s_WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL; // Null-brush to disable WM_ERASEBKGND, in favor of BeginPaint_NoFlicker
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	RegisterClass(&wndclass);

	HWND hwnd = CreateWindowEx(0,
		szAppName, TEXT("Digital Clock"),
		WS_POPUPWINDOW | WS_CLIPCHILDREN, // casual, tune soon
			// note: WS_CLIPCHILDREN avoids re-paint flicking when floatbar is visible and counting down.
		0, 0, 200, 100, // temporal window pos, change later in ReloadIni()
		NULL, NULL, hInstance, 
		this	// MUST preserve this, as LPCREATESTRUCT to WM_NCCREATE
		);

	ReloadIni_and_Redraw(hwnd);

	SetWindowText(hwnd, GetExeFilename());

	ShowWindow(hwnd, SW_NORMAL);
	UpdateWindow(hwnd);

	m_firstUpdateWindowDone = true;

	return TRUE;
}


int CAppWin::clock_cy_from_cx(int cx)
{
	int logicy = LOGICY_HHMMSS + (m_isShowDate ? LOGICY_DATE : 0);
	return cx * logicy / LOGICX_HHMMSS;
}

void CAppWin::my_MoveWindow_byClientRect(HWND hwnd, const RECT& rcClient)
{
	static struct StyleBits
	{
		DWORD bits_on; DWORD bits_on_ex;
	}
	s_two_styles[2] =
	{
		{ WS_POPUPWINDOW | WS_THICKFRAME , WS_EX_DLGMODALFRAME }, // style bits for no-title window
		{ WS_OVERLAPPEDWINDOW , WS_EX_TOOLWINDOW }, // style bits for has-title window
	};

	bool istitle = m_is_show_title;
	UINT winstyle = Hwnd_TuneWinStyleBits(hwnd, s_two_styles[istitle].bits_on, s_two_styles[!istitle].bits_on);
	UINT exstyle = Hwnd_TuneWinStyleExBits(hwnd, s_two_styles[istitle].bits_on_ex, s_two_styles[!istitle].bits_on_ex);
	MoveWindow_byClientRect(hwnd, &rcClient, winstyle, exstyle);
}

bool CAppWin::ClientRectFromINI(HWND hwnd, RECT *prect)
{
	RECT &r = *prect;
	r = m_dxClientRect;

	// Check if INI-provided RECT is empty
	if(RECTcx(r)<=0 || RECTcy(r)<=0)
		return false;

	RECT rectfix = mumo_PlaceRectInsideScreen(r);
	if (rectfix != r)
	{
		vaDBG2(_T("DigClock2 client-area is outside of monitors, fix it from %s to %s"), 
			RECTtext(r).c_str(), RECTtext(rectfix).c_str());
		
		my_MoveWindow_byClientRect(hwnd, rectfix);
		r = rectfix;
	}

	return true;
}

void CAppWin::ReloadIni_and_Redraw(HWND hwnd)
{
	static TCHAR exedir_ini[MAX_PATH] = {};
	static TCHAR userdir[MAX_PATH] = {}, userdir_ini[MAX_PATH] = {};

	if(!exedir_ini[0] || !userdir_ini[0])
	{
		snTprintf(exedir_ini, _T("%s\\%s.ini"), GetExeDir(), GetIniStemName());

		SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, userdir);
		assert(userdir[0]);
		snTprintf(userdir_ini, _T("%s\\%s.ini"), userdir, GetIniStemName());
	}

	const TCHAR* const ar_inifiles[] = { exedir_ini, userdir_ini };
	gp.xini->LoadIni(ar_inifiles, ARRAYSIZE(ar_inifiles), true, DataXIni::LoadFinalFound);

	// Reposition DigClock window according to new INI content.

	const int default_client_width_with_dpi = AfterDpiScale(g_init_client_cx);

	RECT clirect = {};
	bool iniok = ClientRectFromINI(hwnd, &clirect);

	if (!iniok) // INI is empty
	{
		POINT mousepos = {};
		GetCursorPos(&mousepos);

		clirect.left = mousepos.x;
		clirect.top = mousepos.y;

		clirect.right = clirect.left + default_client_width_with_dpi;
		clirect.bottom = clirect.top + clock_cy_from_cx(default_client_width_with_dpi);
	}

	my_MoveWindow_byClientRect(hwnd, clirect);

	Hwnd_SetAlwaysOnTop(hwnd, m_is_always_on_top);
}


class DelaySaveIniTimer : public CHwndTimer
{
public:
	virtual void TimerCallback() cxx11_override
	{
		vaDBG2(_T("[DigClock2] DelaySaveIniTimer triggered..."));

		gp.xini->SaveIni();
	}
} s_DelaySaveIniTimer;


void CAppWin::my_AdjustClientRect(HWND hwnd, bool is_default_width)
{
	RECT clirect = {};
	GetClientRect_ScreenPos(hwnd, &clirect);

	int width = 0;
	if(is_default_width)
	{
		width = AfterDpiScale(g_init_client_cx);
	}
	else
	{
		width = m_iso_client_cx;
	}

	clirect.right = clirect.left + width;
	clirect.bottom = clirect.top + clock_cy_from_cx(width);
	my_MoveWindow_byClientRect(hwnd, clirect);
}


COLORREF SwitchDigitColor(int shift)
{
	static int s_idxcolor = 0;
	static COLORREF s_colors[] =
	{
		RGB(0x40, 0xA0, 0xFF), // sky blue
		RGB(0x00, 0xA8, 0x58), // pond green
		RGB(0xD4, 0x62, 0x62), // dark red
		RGB(0xC0, 0x30, 0xFF), // shining purple
		RGB(0x70, 0x42, 0x14), // brown
		RGB(0xF8, 0x60, 0x30), // deep orange
		RGB(0x30, 0xA0, 0xD0), // dyeing blue
		RGB(0xff, 0x00, 0xff), // magenta
		RGB(0xff, 0x00, 0x00), // full red
	};

	int total = ARRAYSIZE(s_colors);

	int idxnew = s_idxcolor + shift;
	if(idxnew>=total)
		idxnew = 0;
	else if(idxnew<0)
		idxnew = total-1;
	
	s_idxcolor = idxnew;
	
	return s_colors[s_idxcolor];
}

inline int Digit_ScaleDown(int value)
{
	// Chj: For Second-value(not Hour, Minute value), we scale down the 
	// digit display size to 75% to make the seconds value stands out.
	return value * 3 /4;
}

void DisplayDigit (HDC hdc, int iNumber, bool is_scale_down=false)
{
	static BOOL  fSevenSegment [10][7] = {
		1, 1, 1, 0, 1, 1, 1,     // 0
		0, 0, 1, 0, 0, 1, 0,     // 1
		1, 0, 1, 1, 1, 0, 1,     // 2
		1, 0, 1, 1, 0, 1, 1,     // 3
		0, 1, 1, 1, 0, 1, 0,     // 4
		1, 1, 0, 1, 0, 1, 1,     // 5
		1, 1, 0, 1, 1, 1, 1,     // 6
		1, 0, 1, 0, 0, 1, 0,     // 7
		1, 1, 1, 1, 1, 1, 1,     // 8
		1, 1, 1, 1, 0, 1, 1      // 9
	} ;  

	const int NVERTEX = 6;
	static POINT ptSegment [7][NVERTEX] = {
		7,  6,  11,  2,  31,  2,  35,  6,  31, 10,  11, 10,
		6,  7,  10, 11,  10, 31,   6, 35,   2, 31,   2, 11,
		36,  7,  40, 11,  40, 31,  36, 35,  32, 31,  32, 11,
		7, 36,  11, 32,  31, 32,  35, 36,  31, 40,  11, 40,
		6, 37,  10, 41,  10, 61,   6, 65,   2, 61,   2, 41,
		36, 37,  40, 41,  40, 61,  36, 65,  32, 61,  32, 41,
		7, 66,  11, 62,  31, 62,  35, 66,  31, 70,  11, 70 
	} ;

	POINT arPtVertex[NVERTEX] = {};

	int          iSeg ;

	for (iSeg = 0 ; iSeg < ARRAYSIZE(ptSegment) ; iSeg++)
	{
		if (fSevenSegment [iNumber][iSeg])
		{
			POINT *pVertex = ptSegment[iSeg];

			if(is_scale_down)
			{
				for(int j=0; j<NVERTEX; j++)
				{
					arPtVertex[j].x = Digit_ScaleDown(ptSegment[iSeg][j].x);
					arPtVertex[j].y = Digit_ScaleDown(ptSegment[iSeg][j].y);
				}
				pVertex = arPtVertex;
			}

			Polygon (hdc, pVertex, NVERTEX) ;
		}
	}
}

void DisplayTwoDigits (HDC hdc, int iNumber, BOOL fSuppress, bool is_scale_down=false)
{
	// fSuppress: suppress high digit. If true, 1 will be displayed as 1, instead of 01.

	if (!fSuppress || (iNumber / 10 != 0))
		DisplayDigit (hdc, iNumber / 10, is_scale_down) ;

	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
	
	DisplayDigit (hdc, iNumber % 10, is_scale_down) ;
	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
}

void DisplayColon (HDC hdc)
{
	POINT ptColon [2][4] = { 
		2,21,  6,17,  10,21,  6,25,
		2,51,  6,47,  10,51,  6,55 
	} ;

	Polygon (hdc, ptColon [0], 4) ;
	Polygon (hdc, ptColon [1], 4) ;

	OffsetWindowOrgEx (hdc, -12, 0, NULL) ;
}

void DisplayWallTime (HDC hdc, BOOL f24Hour, BOOL fSuppress)
{
	SYSTEMTIME st = {};
	GetLocalTime (&st) ;

	if (f24Hour)
		DisplayTwoDigits (hdc, st.wHour, fSuppress) ;
	else
		DisplayTwoDigits (hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress) ;

	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, st.wMinute, FALSE) ;
	
	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, st.wSecond, FALSE, true) ;
}

void CAppWin::DisplayCountDown(HDC hdc)
{
	HBRUSH hbrushGray = CreateSolidBrush(RGB(220,220,220));
	HBRUSH hbrushOrig = NULL;

	//
	// Drawing clock digits
	//

	if(m_seconds_remain==0)
	{
		// Show gray digits to imply count-down finished.
		hbrushOrig = (HBRUSH) SelectObject(hdc, hbrushGray);
	}

	int zSeconds = m_seconds_remain % 60;
	int tmp = m_seconds_remain / 60;
	int zMinutes = tmp % 60;
	int zHours = (tmp / 60) % 100;
	
	DisplayTwoDigits(hdc, zHours, FALSE);
	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, zMinutes, FALSE) ;
	DisplayColon (hdc) ;
	DisplayTwoDigits (hdc, zSeconds, FALSE, true) ;

	//
	// Drawing the countdown bar.
	//

	OffsetWindowOrgEx(hdc, 42+42, -58, NULL);

	const RECT rcBar = {0,0, 42+42-4, 12};
		
	if(hbrushOrig==NULL)
	{
		hbrushOrig = (HBRUSH) SelectObject(hdc, hbrushGray);
	}
	
	// draw background gray bar
	Rectangle(hdc, rcBar.left, rcBar.top, rcBar.right, rcBar.bottom); 

	if(m_seconds_remain>0)
	{
		// draw remain percent using original color(as user picked)
		SelectObject(hdc, hbrushOrig);

		double frac = (double)m_seconds_remain/m_seconds_countdown_cfg;
		int draw_width = (int)((rcBar.right-rcBar.left)*frac) + 1;
		Rectangle(hdc, rcBar.left, rcBar.top, min(rcBar.left+draw_width, rcBar.right), rcBar.bottom); 
	}

	DeleteObject(hbrushGray);
}

void CAppWin::RefreshTheClock(HDC hdc)
{
	COLORREF digitcolor = m_dxDigitColor;
	HBRUSH hbrush = CreateSolidBrush(digitcolor);

	SelectObject (hdc, GetStockObject (NULL_PEN)) ;
	SelectObject (hdc, hbrush) ;

	if(m_ClockMode==CM_WallTime)
		DisplayWallTime (hdc, m_f24Hour, m_fSuppressHighDigit) ;
	else if(m_ClockMode==CM_Countdown)
		DisplayCountDown(hdc);
	else
		assert(0);

	SelectObject(hdc, GetStockBrush(WHITE_BRUSH));
	DeleteObject(hbrush);
}

void CAppWin::RefreshDateBar(HDC hdc)
{
	if(!m_isShowDate)
		return;

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	TCHAR szDate[40];
	_sntprintf_s(szDate, _TRUNCATE, _T("%04d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);

	if(m_isShowTimezone)
	{
		TCHAR tzs[16];
		int minutes = util_GetTimeZoneOffset();
		if(minutes%60==0)
		{
			// "UTC+8" , "UTC-10" etc
			_sntprintf_s(tzs, _TRUNCATE, _T("UTC%+d"), minutes/60);
		}
		else
		{
			// "+03:30" , "-03:30" etc, adding "UTC" prefix will be too long
			_sntprintf_s(tzs, _TRUNCATE, _T("%+03d:%02d"), minutes/60, abs(minutes)%60);
		}

		_sntprintf_s(szDate, _TRUNCATE, _T("%s(%s)"), szDate, tzs);
	}

	LOGFONT lf = {};
	lf.lfHeight = - LOGICY_DATE;
	lf.lfWeight = 900; // make it bold
	lf.lfPitchAndFamily = FIXED_PITCH|FF_ROMAN;
	HFONT hfont = CreateFontIndirect(&lf);
	assert(hfont);
	SelectObject(hdc, hfont);

	RECT rc = {0, LOGICY_HHMMSS,  LOGICX_HHMMSS, LOGICY_HHMMSS+LOGICY_DATE};

	// Set background/foreground color, the inverse of HH:MM:SS

	COLORREF digitcolor = m_dxDigitColor;
	HBRUSH hbrush = CreateSolidBrush(digitcolor);
	FillRect(hdc, &rc, hbrush);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255,255,255));
	DrawText(hdc, szDate, (int)_tcslen(szDate), &rc, DT_CENTER);

	SelectObject(hdc, GetStockFont(SYSTEM_FONT));
	DeleteObject(hfont);
	SelectObject(hdc, GetStockBrush(WHITE_BRUSH));
	DeleteObject(hbrush);

}

void CAppWin::winenv_ReloadSetting(HWND hwnd)
{
	TCHAR         szBuffer [2] ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2) ;
	m_f24Hour = (szBuffer[0] == '1') ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2) ;
	m_fSuppressHighDigit = (szBuffer[0] == '0') ;

	InvalidateRect (hwnd, NULL, TRUE) ;
}

void Show_CountdownCfg()
{
	ShowWindow(gp.hdlgFloatbar, SW_SHOW);

	HWND hedit = GetDlgItem(gp.hdlgFloatbar, IDC_EDIT_COUNTDOWN);
	SetFocus(hedit);
}

void Hide_CountdownCfg()
{
	ShowWindow(gp.hdlgFloatbar, SW_HIDE);

	HWND hwndMain = GetParent(gp.hdlgFloatbar);
	SetFocus(hwndMain); // so that user can use keyboard arrow to move hwndMain
}

bool CAppWin::StartCountDown(const TCHAR *cfghms)
{
	m_winshaker.ShakeStop();

	int seconds = HMS_to_Seconds(cfghms, true);
	if (seconds < 0)
		return false;

	m_seconds_countdown_cfg = seconds;
	
	m_msectick_start = GetTickCount();

	m_seconds_remain = 1; // set to arbitrary >0 value, will recalculate in DoTimer()
	DoTimer(m_hwnd, ID_TIMER_SECONDS_TICK);

	// Restart timer to align timing boundary.
	SetTimer(m_hwnd, ID_TIMER_SECONDS_TICK, MY_TIMER_INTERVAL_1000ms, NULL);

	InvalidateRect(m_hwnd, NULL, TRUE);

	Hide_CountdownCfg();

	return true;
}

void CAppWin::do_CountdownDone(HWND hwnd, bool isAlert) // Time due
{
	SetDlgItemText(gp.hdlgFloatbar, IDB_StartCountDown, _T("Start countdown"));

	if(!isAlert)
		return;

	if (IsIconic(hwnd)) {
		ShowWindow(hwnd, SW_RESTORE);
	}
	BringWindowToTop(hwnd);
	SetForegroundWindow(hwnd);

	// Shake the window if user wants it.
	if(m_timedue_shake_seconds!=0)
	{
		const int nudge_max = AfterDpiScale(SHAKE_NUDGE_MAX);
		m_winshaker.ShakeStart(hwnd, nudge_max, SHAKE_TIMER_INTERVAL, 
			m_timedue_shake_seconds*1000);
	}

	// Play sound if user wants it.
	if(m_is_playsound)
	{
		auto &chimeplay = *gp.chimeplay;

		Sdring fullpath = _T("");
		const TCHAR *relapath = m_playsound_filepath.GetValue();
		if(relapath && relapath[0])
			 fullpath = GetFullpathRelaToExe(relapath);

		auto pserr = chimeplay.PlayOnce(ChimePlay::TimeDue, fullpath);
		if (pserr)
		{	
			vaDBG1(_T("[INFO] Playing sound file '%s' failed. Fallback to playing default chime."));
			chimeplay.PlayOnce(ChimePlay::TimeDue, nullptr);
		}
	}
}


void CAppWin::DoTimer(HWND hwnd, int idtimer)
{
	if(idtimer==ID_TIMER_SECONDS_TICK)
	{
		int prev_remain = m_seconds_remain;

		if(m_seconds_remain>0)
		{
			// Yes, even if the UI is in walltime mode, we let the countdown go,
			// so that the user can temporarily switch back to peek the walltime. 

			DWORD msectick_end = m_msectick_start + m_seconds_countdown_cfg*1000 - LESS_1millisec;

			DWORD now_msectick = GetTickCount();

			int msec_remain = int(msectick_end - now_msectick);

			if(msec_remain>0)
			{
				// So that remaining 1ms ~ 999ms all considered "remaining one second".
				m_seconds_remain = msec_remain / 1000 + 1;
			}
			else 
			{
				m_seconds_remain = 0;
			}

			assert(m_seconds_remain>=0);

			if(prev_remain>0 && m_seconds_remain==0)
			{
				do_CountdownDone(hwnd, true);
			}

			InvalidateRect(hwnd, NULL, TRUE); // Redraw UI according to m_seconds_remain.

		} // m_seconds_remain>0		
		else if(m_ClockMode==CM_WallTime)
		{
			InvalidateRect(hwnd, NULL, TRUE); // Redraw current time.
		}

	}
	else if(idtimer==ID_TIMER_HIDE_CFG_PANEL)
	{
		if(!Is_MouseInClientRect(hwnd))
			Hide_CountdownCfg();
	}
}

BOOL CAppWin::Cls_OnCreate(HWND hwnd, LPCREATESTRUCT)
{
	SetTimer(hwnd, ID_TIMER_SECONDS_TICK, MY_TIMER_INTERVAL_1000ms, NULL) ;
	SetTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL, 500, NULL);

	m_mouselvp.SetHwnd(hwnd);

	winenv_ReloadSetting(hwnd);

	bool succ = m_tooltip.Create(hwnd, false);
	assert(succ);

	HRSRC hRsrc = FindResource(g_hInstance, MAKEINTRESOURCE(IDR_WAV_STOCK), _T("WAV"));
	assert(hRsrc);
	HGLOBAL hResource = LoadResource(g_hInstance, hRsrc);
	assert(hResource);
	const void *ptrWavBin = LockResource(hResource);
	int bytesWavBin = SizeofResource(g_hInstance, hRsrc);
	s_msgval_playsound_done = gp.chimeplay->SetDefaultChime(ptrWavBin, bytesWavBin, hwnd);
	assert(s_msgval_playsound_done>0);

	assert(!m_hmenuRootPopup);
	m_hmenuRootPopup = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAIN_MENU));
	m_hmenuRootPopup = GetSubMenu(m_hmenuRootPopup, 0);

	CMenuTracker::ReCode_et mterr = m_menu_tracker.BindMenuTree(m_hmenuRootPopup);
	assert(!mterr);
	mterr = m_menu_tracker.AddPopAction(_T("ShowDate"), new CMenuPop_ShowDate);
	assert(!mterr);
	mterr = m_menu_tracker.AddPopAction(CMenuTracker::s_root_popname, new CMenuPop_Root);
	assert(!mterr);
	mterr = m_menu_tracker.AddPopAction(_T("ShakeWindow"), new CMenuPop_ShakeWindow);
	assert(!mterr);
	mterr = m_menu_tracker.AddPopAction(_T("PlaySound"), new CMenuPop_PlaySound);
	assert(!mterr);

	DragAcceptFiles(hwnd, TRUE);

	return TRUE; // create ok
}

void CAppWin::OnWinMoveSize(HWND hwnd)
{
	if(m_firstUpdateWindowDone && !m_winshaker.IsTicking())
	{
		// Do INI saving only AFTER Main window is shown.

		RECT rect = { m_axClient, m_ayClient, m_axClient+m_cxClient, m_ayClient+m_cyClient };

		// Note: On Win10, When a window is minimized, we can see rect.left=rect.top=-32000,
		// and rect.right, rect.bottom may be (-31770,-31902), 
		// So, check for improper rect values before saving to INI.

		if(!(rect.left==-32000 && rect.top==-32000) && RECTcx(rect)>0 && RECTcx(rect)>0)
		{
			m_dxClientRect = rect;
		}

		s_DelaySaveIniTimer.StartDelayedWork(hwnd, DELAY_SAVE_INI_MILLISEC);
	}
}

void CAppWin::Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	m_cxClient = cx;
	m_cyClient = cy;

	vaDBG2(_T("compare m_cxClient, m_iso_client_cx: %d vs %d"), m_cxClient, m_iso_client_cx);
	OnWinMoveSize(hwnd);
}

void CAppWin::Cls_OnMove(HWND hwnd, int x, int y)
{
	POINT abspos = { 0, 0 };
	ClientToScreen(hwnd, &abspos);
	m_axClient = abspos.x; m_ayClient = abspos.y;

	OnWinMoveSize(hwnd);
}

void CAppWin::Cls_OnTimer(HWND hwnd, UINT id)
{
	DoTimer(hwnd, id);
}

void CAppWin::Cls_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {};
	HDC hdc = BeginPaint_NoFlicker(hwnd, &ps) ;
	RECT rccli = {};
	GetClientRect(hwnd, &rccli);
	FillRect(hdc, &rccli, GetStockBrush(WHITE_BRUSH));

	const int logicx = LOGICX_HHMMSS;
	const int logicy = clock_cy_from_cx(logicx);

	SetMapMode (hdc, MM_ISOTROPIC) ;
	SetWindowExtEx (hdc, logicx, logicy, NULL) ;
	SetViewportExtEx (hdc, m_cxClient, m_cyClient, NULL) ;

	SetWindowOrgEx (hdc, logicx/2-1, logicy/2-1, NULL) ;
	SetViewportOrgEx (hdc, m_cxClient / 2, m_cyClient / 2, NULL) ;

	// learn by debug
	SIZE extWinChk = {}, extVptChk = {};
	GetWindowExtEx(hdc, &extWinChk);   // [WinXP] extWinChk will always =SetWindowExtEx's
	assert(extWinChk.cx==logicx && extWinChk.cy==logicy);
	GetViewportExtEx(hdc, &extVptChk); // [WinXP] one of extVptChk.cx, extVptChk.cy may differ to SetViewportExtEx's
	//
	m_iso_client_cx = extVptChk.cx;

	RefreshDateBar(hdc);
	RefreshTheClock(hdc); 

	EndPaint_NoFlicker(hwnd, &ps) ;
}

void CAppWin::Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	gp.chimeplay->PlayStop();

	if(m_winshaker.IsTicking())
	{ 
		m_winshaker.ShakeStop();
		return;
	}

	m_is_dragging = true;
	m_is_moved = false;
	SetCapture(hwnd);

	m_pos_mousedown.x = x;
	m_pos_mousedown.y = y;
}

void CAppWin::Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	bool isCtrl = GetKeyState(VK_CONTROL) < 0;
	bool isShift = GetKeyState(VK_SHIFT) < 0;

	m_is_dragging = false;
	ReleaseCapture();

	if (m_is_moved == false)
	{
		if (m_is_change_color)
		{
			m_dxDigitColor = SwitchDigitColor(isCtrl || isShift ? -1 : 1);
			InvalidateRect(hwnd, NULL, TRUE);

			s_DelaySaveIniTimer.StartDelayedWork(hwnd, DELAY_SAVE_INI_MILLISEC);
		}
	}
}

void CAppWin::Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	// Handle window dragging

	if(m_is_dragging==true)
	{
		// Set new window pos

		int offsetx = x - m_pos_mousedown.x;
		int offsety = y - m_pos_mousedown.y;
		if(offsetx!=0 || offsety!=0)
		{
			m_is_moved = true;
		}

//		vaDbg(_T("pos_mousedown: %d,%d => offset %d,%d"), s_pos_mousedown.x, s_pos_mousedown.y, offsetx, offsety);

		MoveWindow_byOffset(hwnd, offsetx, offsety);
	}

	// Handle Countdown-cfg panel show/hide

	CWmMouseleaveHelper::Move_ret moveret = m_mouselvp.do_WM_MOUSEMOVE();
	if(moveret==CWmMouseleaveHelper::JustEntered)
	{
		if(m_ClockMode==CM_Countdown)
			Show_CountdownCfg();
	}
}

void CAppWin::My_OnMouseLeave(HWND hwnd)
{
	// Here, we check whether mouse pointer is outside the main window.
	// If outside, we will hide the Countdown-cfg panel.
	// Note: If the mouse is over the cfg panel, WM_MOUSELEAVE *is* generated,
	// but we should *not* consider it outside(=should not hide the panel).

	m_mouselvp.do_WM_MOUSELEAVE();

	if(!Is_MouseInClientRect(hwnd)) // mouse outside
	{
		Hide_CountdownCfg();
	}
}

void CAppWin::Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	bool isCtrl = GetKeyState(VK_CONTROL)<0;
	bool isShift = GetKeyState(VK_SHIFT)<0;

	if(fDown)
	{
		int scale = isCtrl ? 10 : 1;

		int offsetx = 0, offsety = 0;
		if(vk==VK_UP)
			offsety = -1 * scale;
		else if(vk==VK_DOWN)
			offsety = 1 * scale;
		else if(vk==VK_LEFT)
			offsetx = -1 * scale;
		else if(vk==VK_RIGHT)
			offsetx = 1 * scale;

		MoveWindow_byOffset(hwnd, offsetx, offsety);
	}
}

void CAppWin::Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	POINT point = {x, y};
	ClientToScreen (hwnd, &point) ;
	TrackPopupMenu(m_hmenuRootPopup, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL) ;
}

void CAppWin::Cls_OnInitMenuPopup(HWND hwnd, HMENU hmenuPopup, UINT item, BOOL fSystemMenu)
{
	m_menu_tracker.Do_WM_INITMENUPOPUP(hwnd, hmenuPopup, item, fSystemMenu);
}

void CAppWin::Cls_OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hSubmenu, UINT flags)
{
	if(hmenu==NULL)
	{
		// Popup-menu just collapses. (User clicks outside any menu item to close the menu.)
		assert(flags==(UINT)-1);

		m_tooltip.Hide();
		gp.chimeplay->PlayStop();
		return;
	}

	m_menu_tracker.Do_WM_MENUSELECT(hwnd, hmenu, item, hSubmenu, flags);
}

void CAppWin::Cls_OnDropFiles(HWND hwnd, HDROP hdrop)
{
	TCHAR audio_path[MAX_PATH] = {};
	int file_count = DragQueryFile(hdrop, -1, NULL, 0);
	assert(file_count>0);

	Sdrings ssfile(file_count);
	for(int i=0; i<file_count; i++)
	{
		int pathlen = DragQueryFile(hdrop, i, NULL, 0);
		ssfile[i].setbufsize(pathlen);
		DragQueryFile(hdrop, i, ssfile[i].getbuf(), ssfile[i].rawlen()+1);
	}
	
	DragFinish(hdrop); // free system-allocated memory for this drag-event

	AddNewFiles_to_ChimeList(hwnd, ssfile);
}

void CAppWin::Cls_OnCommand(HWND hwnd, int cmdid, HWND hwndCtl, UINT codeNotify)
{
	if(cmdid==IDM_COUNTDOWN_MODE)
	{
		m_ClockMode = ClockMode_et(!m_ClockMode);

		if(m_ClockMode==CM_WallTime)
			Hide_CountdownCfg();

		InvalidateRect(hwnd, NULL, TRUE);
	}
	else if(cmdid==IDM_CANCEL_COUNTDOWN)
	{
		m_seconds_remain = 0;

		do_CountdownDone(hwnd, false);

		InvalidateRect(hwnd, NULL, TRUE);
	}
	else if(cmdid==IDM_ALWAYS_ON_TOP)
	{
		m_is_always_on_top = !m_is_always_on_top;

		Hwnd_SetAlwaysOnTop(hwnd, m_is_always_on_top);
	}
	else if(cmdid==IDM_CLICK_CHANGE_COLOR)
	{
		m_is_change_color = !m_is_change_color;
	}
	else if(cmdid==IDM_SHOWDATE_NO)
	{
		m_isShowDate = FALSE;
		m_isShowTimezone = FALSE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOWDATE_YES)
	{
		m_isShowDate = TRUE;
		m_isShowTimezone = FALSE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOWDATE_TIMEZONE)
	{
		m_isShowDate = TRUE;
		m_isShowTimezone = TRUE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOW_TITLE)
	{
		m_is_show_title = !m_is_show_title;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_MINIMIZE_WINDOW)
	{
		ShowWindow(hwnd, SW_MINIMIZE);
	}
	else if(cmdid==IDM_HELP)
	{
		ShowHelp(hwnd);
	}
	else if(cmdid==IDM_RESET_SIZE)
	{
		RECT clirect = {};
		GetClientRect_ScreenPos(hwnd, &clirect);
		my_AdjustClientRect(hwnd, true);
	}
	else if(cmdid==IDM_RELOAD_FROM_INI)
	{
		m_tooltip.ShowBelowMouse(_T("ReloadIni_and_Redraw()"));
		m_tooltip.Hide(2000);

		ReloadIni_and_Redraw(hwnd);
	}
	else if(cmdid==IDM_RESET_ALL_SETTINGS)
	{
		m_tooltip.ShowBelowMouse(_T("Reset INI to program default."));
		m_tooltip.Hide(2000);

		gp.xini->ResetDefault();
		ReloadIni_and_Redraw(hwnd);
	}
	else if(cmdid==ID_SHAKE_NOSHAKE) {
		m_timedue_shake_seconds = 0;
	}
	else if(cmdid==ID_SHAKE_1SEC) {
		m_timedue_shake_seconds = 1;
	}
	else if(cmdid==ID_SHAKE_3SEC) {
		m_timedue_shake_seconds = 3;
	}
	else if(cmdid==ID_SHAKE_5SEC) {
		m_timedue_shake_seconds = 5;
	}
	else if(cmdid==ID_SHAKE_FOREVER) {
		m_timedue_shake_seconds = -1;
	}
	else if(cmdid==ID_PLAYSOUND_NONE)
	{
		m_is_playsound = false;
		m_playsound_filepath.SetValue(_T(""));
	}
	else if (cmdid == ID_PLAYSOUND_DEFAULT)
	{
		m_playsound_filepath.SetValue(_T(""));
		m_is_playsound = true;
	}
	else if(cmdid>=ID_PLAYSOUND_DYNA_START && cmdid<ID_PLAYSOUND_DYNA_END_)
	{
		const TCHAR *filepath = nullptr;
		HMENU hmPlaySound = m_menu_tracker.FindPopname(_T("PlaySound"));
		GetMenuitem_UserContext(hmPlaySound, cmdid, MenuitemById, (void**)&filepath);

		Sdring fullpath = GetFullpathRelaToExe(filepath);

		if(fsapi::file_exists(fullpath))
		{
			m_playsound_filepath = Sdring(filepath); 
			// -- Yes, filepath instead of fullpath, bcz I want it to match exact text from INI.
			//    or
			// g_playsound_filepath.SetValue(filepath);
			// -- Note: Casual coding `g_playsound_filepath = filepath;` is WRONG! 
			//    -- will/should get compiler ambiguous error.
			
			m_is_playsound = true;
		}
		else
		{
			FibInput_st si;
			si.szBtnOK = _T("&Yes");
			si.szBtnCancel = _T("&No");
			si.hIcon = LoadIcon(NULL, IDI_QUESTION);
			si.title = _T("Confirm file delete");
			FIB_ret ans = ggt_vaFlexiInfobox(hwnd, &si,
				_T("Remove this non-existing sound file from menu?\r\n\r\n%s"), 
				fullpath.c_str());
			if(ans==FIB_Yes)
			{
				int delpos = cmdid - ID_PLAYSOUND_DYNA_START;

				Sdrings chime_filepaths = chime_list_GetValue();
				chime_filepaths.DeleteAt(delpos, 1);
				chime_list_SetValue(chime_filepaths);
			}
		}
	}
	else if(cmdid==ID_PLAYSOUND_ADDFILE)
	{
		Sdring audio_extnames = StrJoin(g_audio_extnames, ARRAYSIZE(g_audio_extnames), _T("*;"));
		// -- would result in ".wav*;.mp3"

		Sdring arsFilter[] = { Sdring(_T("Audio files;*"))+audio_extnames, _T("All files;*.*") };
		// -- would result in "Audio files;*.wav;*.mp3"

		Sdrings ss = util_GetOpenFilenames(hwnd, arsFilter, ARRAYSIZE(arsFilter),
			nullptr, _T("chime.wav"));
		int count = ss.count();
		if(count>0)
			AddNewFiles_to_ChimeList_SingleDir(hwnd, ss); // will change g_chime_list.
	}
	else if(cmdid==IDM_DO_TEST1)
	{
//		g_winshaker.ShakeStart(hwnd, 20, 25, 2000);

//		g_chimeplay.RepeatOnce();
	}
	else if (cmdid==IDM_DO_TEST2)
	{
//		g_winshaker.ShakeStop();

//		g_chimeplay.PlayStop();
	}
	else if(cmdid==IDM_EXIT)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}
}

void CAppWin::Cls_OnDestroy(HWND hwnd)
{
	KillTimer(hwnd, ID_TIMER_SECONDS_TICK) ;
	KillTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL);
	
	PostQuitMessage (0) ;
}

LRESULT CAppWin::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{{
 		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
 		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
 		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);

		HANDLE_MSG(hwnd, WM_COMMAND, Cls_OnCommand);

		HANDLE_MSG(hwnd, WM_SIZE, Cls_OnSize);
		HANDLE_MSG(hwnd, WM_MOVE, Cls_OnMove);
		HANDLE_MSG(hwnd, WM_TIMER, Cls_OnTimer);

		HANDLE_MSG(hwnd, WM_MOUSEMOVE, Cls_OnMouseMove);
		case WM_MOUSELEAVE:
			My_OnMouseLeave(hwnd);
			return 0;

		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, Cls_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, Cls_OnLButtonUp);
		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Cls_OnRButtonDown);

		HANDLE_MSG(hwnd, WM_KEYDOWN, Cls_OnKey);

		HANDLE_MSG(hwnd, WM_INITMENUPOPUP, Cls_OnInitMenuPopup);
		HANDLE_MSG(hwnd, WM_MENUSELECT, Cls_OnMenuSelect);

		HANDLE_MSG(hwnd, WM_DROPFILES, Cls_OnDropFiles);

		case WM_SETTINGCHANGE: // for user-locale(time format) change
			winenv_ReloadSetting(hwnd);
			return 0;

		default: ;
	}}

	if (message == s_msgval_playsound_done)
	{
		auto playdone = (CPlaySound_u::PlayDone_et)wParam;
		auto &chimeplay = *gp.chimeplay;

		if (chimeplay.IsPlaying())
		{
			vaDBG2(_T("[DigClock2] Got playSound-done. (stale, %s)"), ITCSnv(playdone, CPlaySound_u::PlayDone_itc));
			return 0;
		}
		else
			vaDBG2(_T("[DigClock2] Got playSound-done. (true, %s)"), ITCSnv(playdone, CPlaySound_u::PlayDone_itc));

		if (m_winshaker.IsTicking())
		{
			vaDBG2(_T(". Window is still shaking, so the chime should repeat."));
			chimeplay.RepeatOnce();
		}
		else if (chimeplay.GetPurpose() == ChimePlay::SndPreview)
		{
			if (playdone == CPlaySound_u::PlayDone_Success)
			{
				vaDBG2(_T(". User is previewing sound, so the chime should repeat."));
				chimeplay.RepeatOnce();
			}
		}

		return 0;
	}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}


//////////////////////////////////////////////////////////////////////////


Sdring CAppWin_GetCountDownCfg() {
	return gp.appwin->GetCountDownCfg();
}

bool CAppWin_StartCountDown(const TCHAR *cfghms) {
	return gp.appwin->StartCountDown(cfghms);
}



#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif
