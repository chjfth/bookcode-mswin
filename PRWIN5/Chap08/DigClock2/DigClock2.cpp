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

-----------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <CHHI_DEBUG.h>

#include <tchar.h>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <shlwapi.h>
#include <ShlObj-winxp-patch.h>
#include "resource.h"

#include <vaDbgTs.h>
#include <vaDbgTs_util.h>
#include <CHHI_vaDBG_is_vaDbgTs.h>

#include <CHwndTimer.h>
#include <RECTxy.h>
#include <win32cozy.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/WM_MOUSELEAVE_helper.h>
#include <mswin/Editbox_EnableKbdAdjustIntnum.h>
#include <WinMultiMon.h>

#include <snTprintf.h>

#include <InterpretConst.h>
	using namespace itc;
#include <DataXString.h>
#include <DataXIni.h>

#include "iversion.h"
#include "utils.h"


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


#define MY_TIMER_INTERVAL_1000ms 1000 // request exactly 1 seconds per WM_TIMER callback

#define ID_TIMER_SECONDS_TICK   1
#define ID_TIMER_HIDE_CFG_PANEL 2

#define LESS_1millisec 1

#define LOGICX_HHMMSS 276
#define LOGICY_HHMMSS 72
#define LOGICY_DATE 24  // since v1.8, show a date-bar at bottom

#define DELAY_SAVE_INI_MILLISEC 1000

HINSTANCE g_hInstance;

enum ClockMode_et { CM_WallTime = 0, CM_Countdown = 1 };

struct Format_int_as_HHMMSS {};
struct Format_COLORREF_as_RGB {};

#include "datax.h" // should place it after `ClockMode_et` definition

DataXIni g_xini;

#define INI_SECNAME _T("cfg")

#define MY_DEFINE_AUTOINI(varname, datatype, keyname, default_val) \
	DataXString_AutoSaveIni<datatype> varname(g_xini, INI_SECNAME, _T(keyname), _T(default_val));

#define MY_DEFINE_AUTOINI_FORMAT(varname, datatype, format, keyname, default_val) \
	DataXString_AutoSaveIni<datatype, format> varname(g_xini, INI_SECNAME, _T(keyname), _T(default_val));


MY_DEFINE_AUTOINI(g_ClockMode, ClockMode_et, "ClockMode", "CM_WallTime");

MY_DEFINE_AUTOINI(g_isShowDate,     bool, "IsShowDate", "false");
MY_DEFINE_AUTOINI(g_isShowTimezone, bool, "IsShowTimezone", "false");

MY_DEFINE_AUTOINI(s_is_always_on_top, bool, "AlwaysOnTop", "true");
MY_DEFINE_AUTOINI(s_is_change_color,  bool, "IsClickToChangeColor", "false");
MY_DEFINE_AUTOINI(s_is_show_title,    bool, "IsShowWindowTitle", "false");

MY_DEFINE_AUTOINI_FORMAT(g_seconds_countdown_cfg, int, Format_int_as_HHMMSS, "CountdownCfg", "00:01:00");

DataXString<RECT> g_dxClientRect; 
// -- Do no use _AutoSaveIni for this, so avoid intensive INI writing.
//    When user drags/resize the window, Client-area RECT value would change very frequently.

int g_firstUpdateWindowDone = 0;

int g_seconds_remain = 0;
DWORD g_msectick_start = 0; // value from GetTickCount()

HWND g_hdlgCountdownCfg;

static HMENU s_hmenuRootPopup;

static int s_cxClient, s_cyClient; // Clock window client-area size pixels
static int s_axClient, s_ayClient; // Clock window client-area absolute(screen) position.

int g_iso_client_cx; // maybe less than s_cxClient, if you squeeze clock-window's height.

const int g_init_client_cx = 188; // Default main-window client-area size(96dpi pixels)

static POINT s_pos_mousedown; // client-area position
static bool s_is_dragging = false;
static bool s_is_moved = false;

static int s_idxcolor = 0;
static DataXString<Uint, Format_COLORREF_as_RGB> s_dxDigitColor(_T("RGB(64,160,255)")); // sky blue

BOOL g_f24Hour;
BOOL g_fSuppressHighDigit;

static CWmMouseleaveHelper s_mouselvp;


INT_PTR CALLBACK Dlgproc_CountdownCfg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ReloadIni_and_Redraw(HWND);

void InitOnce()
{
	InitCommonControls();
	// -- WinXP requires this, otherwise, g_hdlgCountdownCfg will be NULL.

	g_xini.AddItem(INI_SECNAME, _T("ClientAreaRect"), &g_dxClientRect);
	// -- This should not be DataXString_AutoSaveIni, to avoid intensive INI-file writing 
	//    when user drag to change window position/size.

	g_xini.AddItem(INI_SECNAME, _T("DigitColor"), &s_dxDigitColor);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	InitOnce();

	static TCHAR szAppName[] = TEXT(APPNAME);
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;
	HACCEL       hAccel;
	g_hInstance = hInstance;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, _T("MYPROGRAM"));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL; // Null-brush to disable WM_ERASEBKGND, in favor of BeginPaint_NoFlicker
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	RegisterClass(&wndclass);

	hwnd = CreateWindowEx(0,
		szAppName, TEXT("Digital Clock"),
		WS_POPUPWINDOW | WS_CLIPCHILDREN, // casual, tune soon
										  // note: WS_CLIPCHILDREN avoids re-paint flicking when floatbar is visible and counting down.
		0, 0, 200, 100, // temporal window pos, change later in ReloadIni()
		NULL, NULL, hInstance, NULL);

	ReloadIni_and_Redraw(hwnd);

	g_hdlgCountdownCfg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_COUNTDOWN_CFG), hwnd, Dlgproc_CountdownCfg);
	assert(g_hdlgCountdownCfg);

	SetWindowText(hwnd, GetExeFilename());

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	g_firstUpdateWindowDone = 1;

	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	assert(hAccel);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			if (!IsDialogMessage(g_hdlgCountdownCfg, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	return (int)msg.wParam;
}


inline int clock_cy_from_cx(int cx)
{
	int logicy = LOGICY_HHMMSS + (g_isShowDate ? LOGICY_DATE : 0);
	return cx * logicy / LOGICX_HHMMSS;
}

void my_MoveWindow_byClientRect(HWND hwnd, const RECT& rcClient)
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

	bool istitle = s_is_show_title;
	UINT winstyle = Hwnd_TuneWinStyleBits(hwnd, s_two_styles[istitle].bits_on, s_two_styles[!istitle].bits_on);
	UINT exstyle = Hwnd_TuneWinStyleExBits(hwnd, s_two_styles[istitle].bits_on_ex, s_two_styles[!istitle].bits_on_ex);
	MoveWindow_byClientRect(hwnd, &rcClient, winstyle, exstyle);
}

static bool ClientRectFromINI(HWND hwnd, RECT *prect)
{
	RECT &r = *prect;
	r = g_dxClientRect;

	// Check if INI-provided RECT is empty
	if(RECTcx(r)<=0 || RECTcy(r)<=0)
		return false;

	RECT rectfix = mumo_PlaceRectInsideScreen(r, true);
	if (rectfix != r)
	{
		vaDBG2(_T("DigClock2 client-area is outside of monitors, fix it from %s to %s"), 
			RECTtext(r).c_str(), RECTtext(rectfix).c_str());
		
		my_MoveWindow_byClientRect(hwnd, rectfix);
		r = rectfix;
	}

	return true;
}

void ReloadIni_and_Redraw(HWND hwnd)
{
	static TCHAR exedir_ini[MAX_PATH] = {};
	static TCHAR userdir[MAX_PATH] = {}, userdir_ini[MAX_PATH] = {};

	if(!exedir_ini[0] || !userdir_ini[0])
	{
		snTprintf(exedir_ini, _T("%s\\%s.ini"), GetExeDir(), GetExeStemname());

		SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, userdir);
		assert(userdir[0]);
		snTprintf(userdir_ini, _T("%s\\%s.ini"), userdir, GetExeStemname());
	}

	const TCHAR* const ar_inifiles[] = { exedir_ini, userdir_ini };
	g_xini.LoadIni(ar_inifiles, ARRAYSIZE(ar_inifiles));

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

	Hwnd_SetAlwaysOnTop(hwnd, s_is_always_on_top);
}


class DelaySaveIniTimer : public CHwndTimer
{
public:
	virtual void TimerCallback() cxx11_override
	{
		vaDBG2(_T("DelaySaveIniTimer triggered..."));

		g_xini.SaveIni();
	}
} s_DelaySaveIniTimer;


void my_AdjustClientRect(HWND hwnd, bool is_default_width=false)
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
		width = g_iso_client_cx;
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

int Digit_ScaleDown(int value)
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

void DisplayCountDown(HDC hdc)
{
	HBRUSH hbrushGray = CreateSolidBrush(RGB(220,220,220));
	HBRUSH hbrushOrig = NULL;

	//
	// Drawing clock digits
	//

	if(g_seconds_remain==0)
	{
		// Show gray digits to imply count-down finished.
		hbrushOrig = (HBRUSH) SelectObject(hdc, hbrushGray);
	}

	int zSeconds = g_seconds_remain % 60;
	int tmp = g_seconds_remain / 60;
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

	if(g_seconds_remain>0)
	{
		// draw remain percent using original color(as user picked)
		SelectObject(hdc, hbrushOrig);

		double frac = (double)g_seconds_remain/g_seconds_countdown_cfg;
		int draw_width = (int)((rcBar.right-rcBar.left)*frac) + 1;
		Rectangle(hdc, rcBar.left, rcBar.top, min(rcBar.left+draw_width, rcBar.right), rcBar.bottom); 
	}

	DeleteObject(hbrushGray);
}

void RefreshTheClock(HDC hdc)
{
	COLORREF digitcolor = s_dxDigitColor;
	HBRUSH hbrush = CreateSolidBrush(digitcolor);

	SelectObject (hdc, GetStockObject (NULL_PEN)) ;
	SelectObject (hdc, hbrush) ;

//	vaDbg(_T("RefreshTheClock()..."));
	if(g_ClockMode==CM_WallTime)
		DisplayWallTime (hdc, g_f24Hour, g_fSuppressHighDigit) ;
	else if(g_ClockMode==CM_Countdown)
		DisplayCountDown(hdc);
	else
		assert(0);

	SelectObject(hdc, GetStockBrush(WHITE_BRUSH));
	DeleteObject(hbrush);
}

void RefreshDateBar(HDC hdc)
{
	if(!g_isShowDate)
		return;

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	TCHAR szDate[40];
	_sntprintf_s(szDate, _TRUNCATE, _T("%04d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);

	if(g_isShowTimezone)
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

	COLORREF digitcolor = s_dxDigitColor;
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

void winenv_ReloadSetting(HWND hwnd)
{
	TCHAR         szBuffer [2] ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2) ;
	g_f24Hour = (szBuffer[0] == '1') ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2) ;
	g_fSuppressHighDigit = (szBuffer[0] == '0') ;

	InvalidateRect (hwnd, NULL, TRUE) ;
}

void Show_CountdownCfg()
{
	ShowWindow(g_hdlgCountdownCfg, SW_SHOW);

	HWND hedit = GetDlgItem(g_hdlgCountdownCfg, IDC_EDIT1);
	SetFocus(hedit);
}

void Hide_CountdownCfg()
{
	ShowWindow(g_hdlgCountdownCfg, SW_HIDE);

	HWND hwndMain = GetParent(g_hdlgCountdownCfg);
	SetFocus(hwndMain); // so that user can use keyboard arrow to move hwndMain
}

void do_CountdownDone(HWND hwnd)
{
	(void)hwnd;

	MessageBeep(MB_OK); // time-up beep

	SetDlgItemText(g_hdlgCountdownCfg, IDB_StartCountDown, _T("Start countdown"));

	if (IsIconic(hwnd)) {
		ShowWindow(hwnd, SW_RESTORE);
	}
	BringWindowToTop(hwnd);
	SetForegroundWindow(hwnd);
}

void DoTimer(HWND hwnd, int idtimer)
{
	if(idtimer==ID_TIMER_SECONDS_TICK)
	{
		int prev_remain = g_seconds_remain;

		if(g_seconds_remain>0)
		{
			// Yes, even if the UI is in walltime mode, we let the countdown go,
			// so that the user can temporarily switch back to peek the walltime. 

			DWORD msectick_end = g_msectick_start + g_seconds_countdown_cfg*1000 - LESS_1millisec;

			DWORD now_msectick = GetTickCount();

			int msec_remain = int(msectick_end - now_msectick);

			if(msec_remain>0)
			{
				// So that remaining 1ms ~ 999ms all considered "remaining one second".
				g_seconds_remain = msec_remain / 1000 + 1;
			}
			else 
			{
				g_seconds_remain = 0;
			}

			assert(g_seconds_remain>=0);

			if(prev_remain>0 && g_seconds_remain==0)
			{
				do_CountdownDone(hwnd);
			}

			InvalidateRect(hwnd, NULL, TRUE); // Redraw UI according to g_seconds_remain.

		} // g_seconds_remain>0		
		else if(g_ClockMode==CM_WallTime)
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

BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	SetTimer(hwnd, ID_TIMER_SECONDS_TICK, MY_TIMER_INTERVAL_1000ms, NULL) ;
	SetTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL, 500, NULL);

	if(!s_hmenuRootPopup)
	{
		s_hmenuRootPopup = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));
		s_hmenuRootPopup = GetSubMenu(s_hmenuRootPopup, 0) ; 
	}

	s_mouselvp.SetHwnd(hwnd);

	winenv_ReloadSetting(hwnd);

	return TRUE; // create ok
}

static void OnWinMoveSize(HWND hwnd)
{
	if(g_firstUpdateWindowDone>0)
	{
		// Launch INI saving only AFTER Main window is shown.

		RECT rect = { s_axClient, s_ayClient, s_axClient + s_cxClient, s_ayClient + s_cyClient };

		// Note: On Win10, When a window is minimized, we can see rect.left=rect.top=-32000,
		// and rect.right, rect.bottom may be (-31770,-31902), 
		// So, check for improper rect values before saving to INI.

		if(!(rect.left==-32000 && rect.top==-32000) && RECTcx(rect)>0 && RECTcx(rect)>0)
		{
			g_dxClientRect.SetValue(rect);
		}

		s_DelaySaveIniTimer.StartDelayedWork(hwnd, DELAY_SAVE_INI_MILLISEC);
	}
}

void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	s_cxClient = cx;
	s_cyClient = cy;

	// vaDbgTs(_T("compare s_cxClient, g_iso_client_cx: %d vs %d"), s_cxClient, g_iso_client_cx);
	OnWinMoveSize(hwnd);
}

void Cls_OnMove(HWND hwnd, int x, int y)
{
	POINT abspos = {0, 0};
	ClientToScreen(hwnd, &abspos);
	s_axClient = abspos.x; s_ayClient = abspos.y;

	OnWinMoveSize(hwnd);
}

void Cls_OnTimer(HWND hwnd, UINT id)
{
	DoTimer(hwnd, id);
}

void Cls_OnPaint(HWND hwnd)
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
	SetViewportExtEx (hdc, s_cxClient, s_cyClient, NULL) ;

	SetWindowOrgEx (hdc, logicx/2-1, logicy/2-1, NULL) ;
	SetViewportOrgEx (hdc, s_cxClient / 2, s_cyClient / 2, NULL) ;

	// learn by debug
	SIZE extWinChk = {}, extVptChk = {};
	GetWindowExtEx(hdc, &extWinChk);   // [WinXP] extWinChk will always =SetWindowExtEx's
	assert(extWinChk.cx==logicx && extWinChk.cy==logicy);
	GetViewportExtEx(hdc, &extVptChk); // [WinXP] one of extVptChk.cx, extVptChk.cy may differ to SetViewportExtEx's
	//
	g_iso_client_cx = extVptChk.cx;

	RefreshDateBar(hdc);
	RefreshTheClock(hdc); 

	EndPaint_NoFlicker(hwnd, &ps) ;
}

void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	s_is_dragging = true;
	s_is_moved = false;
	SetCapture(hwnd);

	s_pos_mousedown.x = x;
	s_pos_mousedown.y = y;
}

void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	// Handle window dragging

	if(s_is_dragging==true)
	{
		// Set new window pos

		int offsetx = x - s_pos_mousedown.x;
		int offsety = y - s_pos_mousedown.y;
		if(offsetx!=0 || offsety!=0)
		{
			s_is_moved = true;
		}

//		vaDbg(_T("pos_mousedown: %d,%d => offset %d,%d"), s_pos_mousedown.x, s_pos_mousedown.y, offsetx, offsety);

		MoveWindow_byOffset(hwnd, offsetx, offsety);
	}

	// Handle Countdown-cfg panel show/hide

	CWmMouseleaveHelper::Move_ret moveret = s_mouselvp.do_WM_MOUSEMOVE();
	if(moveret==CWmMouseleaveHelper::JustEntered)
	{
		if(g_ClockMode==CM_Countdown)
			Show_CountdownCfg();
	}
}

void My_OnMouseLeave(HWND hwnd)
{
	// Here, we check whether mouse pointer is outside the main window.
	// If outside, we will hide the Countdown-cfg panel.
	// Note: If the mouse is over the cfg panel, WM_MOUSELEAVE *is* generated,
	// but we should *not* consider it outside(=should not hide the panel).

	s_mouselvp.do_WM_MOUSELEAVE();

	if(!Is_MouseInClientRect(hwnd)) // mouse outside
	{
		Hide_CountdownCfg();
	}
}

void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	bool isCtrl = GetKeyState(VK_CONTROL)<0;
	bool isShift = GetKeyState(VK_SHIFT)<0;

	s_is_dragging = false;
	ReleaseCapture();

	if(s_is_moved==false)
	{
		if(s_is_change_color)
		{
			s_dxDigitColor = SwitchDigitColor(isCtrl||isShift ? -1 : 1);
			InvalidateRect(hwnd, NULL, TRUE);

			s_DelaySaveIniTimer.StartDelayedWork(hwnd, DELAY_SAVE_INI_MILLISEC);
		}
	}
}

void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
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

void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	POINT point = {x, y};
	ClientToScreen (hwnd, &point) ;
	TrackPopupMenu(s_hmenuRootPopup, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL) ;
}

void Cls_OnInitMenuPopup(HWND hwnd, HMENU hmenuPopup, UINT item, BOOL fSystemMenu)
{
	// The program has two popups, one is s_hmenuRootPopup.
	// the other is "Show Date". We should only care for the very two popup.
	// In other word, various system-menu popups should be ignored.

	if(hmenuPopup==s_hmenuRootPopup)
	{
		CheckMenuItem(hmenuPopup, IDM_COUNTDOWN_MODE,
			g_ClockMode==CM_Countdown ? MF_CHECKED : MF_UNCHECKED);

		EnableMenuItem(hmenuPopup, IDM_STOP_COUNTDOWN,
			g_seconds_remain>0 ? MF_ENABLED: MF_GRAYED);

		CheckMenuItem(hmenuPopup, IDM_ALWAYS_ON_TOP, 
			s_is_always_on_top ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_CLICK_CHANGE_COLOR, 
			s_is_change_color ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOW_TITLE, 
			s_is_show_title ? MF_CHECKED : MF_UNCHECKED);

		return;
	}

	// Now we should find out the "Show Date" popup's menu-handle.
	// I do it everytime dynamically, bcz in the future, the menu text can be in
	// different language, so it will be hard to determine that menu-handle in advance.

	HMENU hmShowDate = FindSubMenu_byText(s_hmenuRootPopup, _T("Show Date"));
	HMENU hmReset    = FindSubMenu_byText(s_hmenuRootPopup, _T("&Reset")); // just debug

	if (hmenuPopup == hmShowDate)
	{
		vaDBG2(_T("See [Show Date] menu popup, hmenu=0x%X"), Ptr2Uint(hmenuPopup));

		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_ID | MIIM_FTYPE;
		BOOL b = GetMenuItemInfo(hmenuPopup, 0, TRUE, &mii);
		assert(mii.wID == IDM_SHOWDATE_NO); // first item show be IDM_SHOWDATE_NO

		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_NO,
			g_isShowDate ? MF_UNCHECKED : MF_CHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_YES,
			(g_isShowDate && !g_isShowTimezone) ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOWDATE_TIMEZONE,
			(g_isShowDate && g_isShowTimezone) ? MF_CHECKED : MF_UNCHECKED);

	}
	else
	{	// Add some debug messages.
		HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
		if (hmenuPopup == hSysMenu)
			vaDBG2(_T("See GetSystemMenu() popup, hmenu=0x%X"), Ptr2Uint(hmenuPopup));
		else if(hmenuPopup == hmReset)
			vaDBG2(_T("See [Reset] menu popup, hmenu=0x%X"), Ptr2Uint(hmenuPopup));
		else
			vaDBG2(_T("Unknown menu popup, hmenu=0x%X"), Ptr2Uint(hmenuPopup));
	}
}

void Cls_OnCommand(HWND hwnd, int cmdid, HWND hwndCtl, UINT codeNotify)
{
	if(cmdid==IDM_COUNTDOWN_MODE)
	{
		g_ClockMode = ClockMode_et(!g_ClockMode);

		if(g_ClockMode==CM_WallTime)
			Hide_CountdownCfg();

		InvalidateRect(hwnd, NULL, TRUE);
	}
	else if(cmdid==IDM_STOP_COUNTDOWN)
	{
		g_seconds_remain = 0;

		do_CountdownDone(hwnd);

		InvalidateRect(hwnd, NULL, TRUE);
	}
	else if(cmdid==IDM_ALWAYS_ON_TOP)
	{
		s_is_always_on_top = !s_is_always_on_top;

		Hwnd_SetAlwaysOnTop(hwnd, s_is_always_on_top);
	}
	else if(cmdid==IDM_CLICK_CHANGE_COLOR)
	{
		s_is_change_color = !s_is_change_color;
	}
	else if(cmdid==IDM_SHOWDATE_NO)
	{
		g_isShowDate = FALSE;
		g_isShowTimezone = FALSE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOWDATE_YES)
	{
		g_isShowDate = TRUE;
		g_isShowTimezone = FALSE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOWDATE_TIMEZONE)
	{
		g_isShowDate = TRUE;
		g_isShowTimezone = TRUE;
		my_AdjustClientRect(hwnd);
	}
	else if(cmdid==IDM_SHOW_TITLE)
	{
		s_is_show_title = !s_is_show_title;
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
	else if(cmdid==ID_ACCEL_ReloadIni)
	{
		ReloadIni_and_Redraw(hwnd);
	}
	else if(cmdid==IDM_RESET_ALL_SETTINGS)
	{
		g_xini.ResetDefault();
		ReloadIni_and_Redraw(hwnd);
	}
	else if(cmdid==IDM_EXIT)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}
}

void Cls_OnDestroy(HWND hwnd)
{
	KillTimer(hwnd, ID_TIMER_SECONDS_TICK) ;
	KillTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL);
	
	PostQuitMessage (0) ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_SIZE, Cls_OnSize);
		HANDLE_MSG(hwnd, WM_MOVE, Cls_OnMove);
		HANDLE_MSG(hwnd, WM_TIMER, Cls_OnTimer);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		
		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, Cls_OnLButtonDown);

		HANDLE_MSG(hwnd, WM_MOUSEMOVE, Cls_OnMouseMove);
		case WM_MOUSELEAVE: 
			My_OnMouseLeave(hwnd);
			return 0;
		HANDLE_MSG(hwnd, WM_LBUTTONUP, Cls_OnLButtonUp);
		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Cls_OnRButtonDown);

		HANDLE_MSG(hwnd, WM_KEYDOWN, Cls_OnKey);

		HANDLE_MSG(hwnd, WM_INITMENUPOPUP, Cls_OnInitMenuPopup);
		HANDLE_MSG(hwnd, WM_COMMAND, Cls_OnCommand);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);

		case WM_SETTINGCHANGE: // for user-locale(time format) change
			winenv_ReloadSetting(hwnd);
			return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

//////////////////////////////////////////////////////////////////////////

BOOL CountdownCfg_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	Sdring cfghms = Seconds_to_HMS(g_seconds_countdown_cfg);
	SetDlgItemText(hDlg, IDC_EDIT1, cfghms);

	HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
	Editbox_EnableKbdAdjustIntnum(hEdit, 0, 59, 1, true, 2);

	// Place editbox caret at end, bcz when debugging, we fiddle with seconds often.
	int textlen = GetWindowTextLength (hEdit);
	Edit_SetSel(hEdit, textlen, textlen);

	// return FALSE to tell dialog manager: Do NOT set keyboard focus to the editbox,
	// bcz, I want the default focus on main-window itself, so that user can move the
	// main-window by keyboard arrow keys.
	return FALSE; 
}

void CountdownCfg_OnCommand(HWND hDlg, int idcmd, HWND hwndCtl, UINT codeNotify)
{
	HWND hwndMain = GetParent(hDlg);

	if(idcmd==IDB_StartCountDown)
	{
		TCHAR szHMS[20] = {};
		GetDlgItemText(hDlg, IDC_EDIT1, szHMS, ARRAYSIZE(szHMS)-1);
		int seconds = HMS_to_Seconds(szHMS, true);
		if(seconds<0)
			return;

		g_seconds_countdown_cfg = seconds;
		g_msectick_start = GetTickCount();

		g_seconds_remain = 1; // arbitrary >0 value, will recalculate in DoTimer()
		DoTimer(hwndMain, ID_TIMER_SECONDS_TICK);

		// Restart timer to align timing boundary.
		SetTimer(hwndMain, ID_TIMER_SECONDS_TICK, MY_TIMER_INTERVAL_1000ms, NULL);

		Hide_CountdownCfg();

		SetDlgItemText(hDlg, IDB_StartCountDown, _T("Restart countdown"));

		InvalidateRect(hwndMain, NULL, TRUE);			
	}
}

INT_PTR CALLBACK 
Dlgproc_CountdownCfg (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_dlgMSG(hwnd, WM_INITDIALOG, CountdownCfg_OnInitDialog);
		HANDLE_dlgMSG(hwnd, WM_COMMAND, CountdownCfg_OnCommand);
	}
	return FALSE; // Let Dlg-manager do default for current message.
}


#ifndef DigClock2_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif
