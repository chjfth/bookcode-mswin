/*-----------------------------------------
DIGCLOCK.c -- Digital Clock(c) Charles Petzold, 1998

DigClock2.cpp -- Updated by Jimm Chen

Since 2022.05:
* Thin window border.
* Drag window by clicking anywhere inside the window.
* Move window by pressing arrow keys, Ctrl to accelerate.
* Right-click context menu, toggle always on top.
* Select different color by mouse clicking.
* Scale down "second" value display(75%), so to have it stand out.
* Toggle showing window title, with EXE file name as title text.
* Eliminate UI flickering by MemDC-double-buffering.

Since 2024.10:
* Add Countdown mode.
-----------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include "resource.h"

#include "iversion.h"
#include "utils.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

void ShowHelp(HWND hwndParent)
{
	static TCHAR *s_help_fmt =
		_T("DigClock2 by Jimm Chen. (version %d.%d)\r\n")
		_T("\r\n")
		_T("This clock program works in wall-time mode or countdown mode.\r\n")
		_T("Clock drawing code by DigClock from Charles Petzold [PRWIN5] Chap08.\r\n")

		_T("\r\n")
		_T("To Move the clock window:\r\n")
		_T("(1) Click and drag with mouse left button.\r\n")
		_T("(2) Use keyboard arrow keys, pixel by pixel. Press Ctrl key to accelerate.\r\n")
		_T("\r\n")
		_T("To change digit color: \r\n")
		_T("(1) Left click on the clock for next color.\r\n")
		_T("(2) Shift+click to cycle back.\r\n")
		_T("\r\n")
		_T("Compiled on: ") _T(__DATE__) _T(", ") _T(__TIME__)
		;
	vaMsgBox(hwndParent, MB_OK,_T("Help"), 
		s_help_fmt, THISEXE_VMAJOR, THISEXE_VMINOR);
}

#define ID_TIMER_SECONDS_TICK   1
#define ID_TIMER_HIDE_CFG_PANEL 2

#define ADD_EXTRA_MILLISEC 16

HINSTANCE g_hInstance;

BOOL g_f24Hour;
BOOL g_fSuppressHighDigit;

enum ClockMode_et { CM_WallTime=0, CM_Countdown=1 } g_ClockMode;

int g_seconds_countdown_cfg = 60;
int g_seconds_remain = 0;
DWORD g_msectick_start = 0; // value from GetTickCount()

HWND g_hdlgCountdownCfg;

POINT g_ptClickCountDown;

SIZE g_init_clisize = {180, 60}; // Initial main-window client-area size

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

INT_PTR CALLBACK Dlgproc_CountdownCfg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT (APPNAME) ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;
	g_hInstance = hInstance;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = NULL; // Null-brush to disable WM_ERASEBKGND, in favor of BeginPaint_NoFlicker
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("Program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	POINT mousepos = {};
	GetCursorPos(&mousepos);

	hwnd = CreateWindowEx (0,
		szAppName, TEXT ("Digital Clock"),
		WS_POPUPWINDOW, // will set more styles in Hwnd_ShowTitle()
		mousepos.x, mousepos.y,
		g_init_clisize.cx, g_init_clisize.cy, // CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL) ;
	Hwnd_ShowTitle(hwnd, false, g_init_clisize.cx, g_init_clisize.cy);

	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance, _T("MYPROGRAM")));

	Hwnd_ShowTitle(hwnd, false);

	g_hdlgCountdownCfg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_COUNTDOWN_CFG), hwnd, Dlgproc_CountdownCfg);
	assert(g_hdlgCountdownCfg);

	SetWindowText(hwnd, GetExeFilename());

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;


	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(!IsDialogMessage(g_hdlgCountdownCfg, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return (int)msg.wParam ;
}

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

int Get_NewColorIdx(int old, int shift)
{
	int total = ARRAYSIZE(s_colors);
	int idxnew = old + shift;
	if(idxnew>=total)
		idxnew = 0;
	else if(idxnew<0)
		idxnew = total-1;
	
	return idxnew;
}

int Digit_ScaleDown(int value)
{
	// Chj: For Second value(not Hour, Minute value), we scale down the 
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


//SetWindowExtEx (hdc, 276*2, 72*2, NULL) ;

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

//SetWindowExtEx (hdc, 276, 72, NULL) ;

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
	//FrameRect(hdc, &rcBar, GetStockBrush(BLACK_BRUSH));
		
	HBRUSH hbrushGray = CreateSolidBrush(RGB(220,220,220));
	HBRUSH hbrushOrig = (HBRUSH) SelectObject(hdc, hbrushGray);
	
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
//	vaDbg(_T("RefreshTheClock()..."));
	if(g_ClockMode==CM_WallTime)
		DisplayWallTime (hdc, g_f24Hour, g_fSuppressHighDigit) ;
	else if(g_ClockMode==CM_Countdown)
		DisplayCountDown(hdc);
	else
		assert(0);
}

void ReloadSetting(HWND hwnd)
{
	TCHAR         szBuffer [2] ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2) ;
	g_f24Hour = (szBuffer[0] == '1') ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2) ;
	g_fSuppressHighDigit = (szBuffer[0] == '0') ;

	InvalidateRect (hwnd, NULL, TRUE) ;
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

			DWORD now_msectick = GetTickCount();
			int msec_remain = int(
				g_msectick_start + g_seconds_countdown_cfg*1000 + ADD_EXTRA_MILLISEC
				- now_msectick);

			if(msec_remain>0)
				g_seconds_remain = (msec_remain+999) / 1000;
			else 
				g_seconds_remain = 0;

			g_seconds_remain--;

			if(prev_remain>0 && g_seconds_remain==0)
				MessageBeep(MB_OK); // time-up beep
		}
		
		InvalidateRect (hwnd, NULL, TRUE); // draw UI according to g_seconds_remain
	}
	else if(idtimer==ID_TIMER_HIDE_CFG_PANEL)
	{
		if(!Is_MouseInClientRect(hwnd))
			ShowWindow(g_hdlgCountdownCfg, SW_HIDE);
	}
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int    cxClient, cyClient ;
	HDC           hdc ;
	PAINTSTRUCT   ps ;

	static HMENU s_popmenu;
	static bool s_is_always_on_top = true;
	static bool s_is_change_color = false;
	static bool s_is_show_title = false;

	static POINT s_pos_mousedown; // client-area position
	static bool s_is_dragging = false;
	static bool s_is_moved = false;
	static int s_idxcolor = 0;

	static bool s_isScratchingMainWindow = false;

	bool isCtrl = GetKeyState(VK_CONTROL)<0;
	bool isShift = GetKeyState(VK_SHIFT)<0;

	switch (message)
	{{
	case WM_CREATE:
	{
		SetTimer(hwnd, ID_TIMER_SECONDS_TICK, 1000, NULL) ;
		SetTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL, 500, NULL);

		if(!s_popmenu)
		{
			s_popmenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_MENU1));
			s_popmenu = GetSubMenu(s_popmenu, 0) ; 
		}

		Hwnd_SetAlwaysOnTop(hwnd, s_is_always_on_top);

		ReloadSetting(hwnd);
		return 0;
	}
	case WM_SETTINGCHANGE:
		ReloadSetting(hwnd);
		return 0 ;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_TIMER:
	{
		WPARAM idtimer = wParam;
		DoTimer(hwnd, (int)idtimer);
		return 0 ;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint_NoFlicker(hwnd, &ps) ;
		RECT rccli = {};
		GetClientRect(hwnd, &rccli);
		FillRect(hdc, &rccli, GetStockBrush(WHITE_BRUSH));

		HBRUSH hbrush = CreateSolidBrush(s_colors[s_idxcolor]);

		SetMapMode (hdc, MM_ISOTROPIC) ;
		SetWindowExtEx (hdc, 276, 72, NULL) ;
		SetViewportExtEx (hdc, cxClient, cyClient, NULL) ;

		SetWindowOrgEx (hdc, 138, 36, NULL) ;
		SetViewportOrgEx (hdc, cxClient / 2, cyClient / 2, NULL) ;

		SelectObject (hdc, GetStockObject (NULL_PEN)) ;
		SelectObject (hdc, hbrush) ;

		RefreshTheClock(hdc); 

		EndPaint_NoFlicker(hwnd, &ps) ;

		SelectObject(hdc, GetStockBrush(WHITE_BRUSH));
		DeleteObject(hbrush);

		return 0 ;
	}

	case WM_LBUTTONDOWN:
	{
		s_is_dragging = true;
		s_is_moved = false;
		SetCapture(hwnd);

		s_pos_mousedown.x = GET_X_LPARAM(lParam);
		s_pos_mousedown.y = GET_Y_LPARAM(lParam);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		bool hit = false;

		// Handle window dragging
		//

		if(s_is_dragging==true)
		{
			// Set new window pos

			int offsetx = GET_X_LPARAM(lParam) - s_pos_mousedown.x;
			int offsety = GET_Y_LPARAM(lParam) - s_pos_mousedown.y;
			if(offsetx!=0 || offsety!=0)
			{
				s_is_moved = true;
			}

			MoveWindow_byOffset(hwnd, offsetx, offsety);
			hit = true;
		}

		//
		// Handle Countdown-cfg panel show/hide
		// 

		if(!s_isScratchingMainWindow)
		{
			// Establish WM_MOUSELEAVE tracking.
			TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd};
			TrackMouseEvent(&tme);

			POINT ptnow = {};
			GetCursorPos(&ptnow);

			if(g_ClockMode==CM_Countdown)
			{
				if(!(ptnow.x==g_ptClickCountDown.x && ptnow.y==g_ptClickCountDown.y))
				{
					s_isScratchingMainWindow = true;

//					vaDbg(_T("Re-show cfgdlg."));
					ShowWindow(g_hdlgCountdownCfg, SW_SHOW);
				}
			}

			hit = true;
		}

		//
		if(hit)
			return 0; // Not calling DefWindowProc()
		else
			break;
	}

	case WM_MOUSELEAVE:
	{
		// Here, we check whether mouse pointer is outside the main window.
		// If outside, we will hide Countdown-cfg panel.
		// Note: If the mouse is over the cfg panel, WM_MOUSELEAVE *is* generated,
		// but we should *not* consider it outside(=should not hide the panel).

		s_isScratchingMainWindow = false;

		if(!Is_MouseInClientRect(hwnd)) // mouse outside
		{
			ShowWindow(g_hdlgCountdownCfg, SW_HIDE);
		}

		break;
	}

	case WM_LBUTTONUP:
	{
		s_is_dragging = false;
		ReleaseCapture();

		if(s_is_moved==false)
		{
			if(s_is_change_color)
			{
				s_idxcolor = Get_NewColorIdx(s_idxcolor, (isCtrl||isShift) ? -1 : 1);
				InvalidateRect(hwnd, NULL, TRUE);
			}
		}

		return 0;
	}

	case WM_RBUTTONDOWN:
	{
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		ClientToScreen (hwnd, &point) ;
		TrackPopupMenu(s_popmenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL) ;
		return 0;
	}

	case WM_KEYDOWN:
	{
		int scale = isCtrl ? 10 : 1;

		int offsetx = 0, offsety = 0;
		if(wParam==VK_UP)
			offsety = -1 * scale;
		else if(wParam==VK_DOWN)
			offsety = 1 * scale;
		else if(wParam==VK_LEFT)
			offsetx = -1 * scale;
		else if(wParam==VK_RIGHT)
			offsetx = 1 * scale;

		MoveWindow_byOffset(hwnd, offsetx, offsety);

		return 0;
	}

	case WM_INITMENUPOPUP:
	{
		HMENU hmenuPopup = (HMENU)wParam;
		if(hmenuPopup!=s_popmenu)
			break;

		CheckMenuItem(hmenuPopup, IDM_COUNTDOWN_MODE,
			g_ClockMode==CM_Countdown ? MF_CHECKED : MF_UNCHECKED);

		EnableMenuItem(hmenuPopup, IDM_STOP_COUNTDOWN,
			g_seconds_remain>0 ? MF_ENABLED: MF_DISABLED);

		CheckMenuItem(hmenuPopup, IDM_ALWAYS_ON_TOP, 
			s_is_always_on_top ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_CLICK_CHANGE_COLOR, 
			s_is_change_color ? MF_CHECKED : MF_UNCHECKED);

		CheckMenuItem(hmenuPopup, IDM_SHOW_TITLE, 
			s_is_show_title ? MF_CHECKED : MF_UNCHECKED);

		return 0;
	}

	case WM_COMMAND:
	{
		int cmdid = GET_WM_COMMAND_ID(wParam, lParam);

		if(cmdid==IDM_COUNTDOWN_MODE)
		{
			g_ClockMode = ClockMode_et(!g_ClockMode);

			if(g_ClockMode==CM_WallTime)
				ShowWindow(g_hdlgCountdownCfg, SW_HIDE);

			InvalidateRect(hwnd, NULL, TRUE);
		}
		else if(cmdid==IDM_STOP_COUNTDOWN)
		{
			g_seconds_remain = 0;
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
		else if(cmdid==IDM_SHOW_TITLE)
		{
			s_is_show_title = !s_is_show_title;
			Hwnd_ShowTitle(hwnd, s_is_show_title);
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
			Hwnd_ShowTitle(hwnd, s_is_show_title, g_init_clisize.cx, g_init_clisize.cy);
		}
		else if(cmdid==IDM_EXIT)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}

		return 0;
	}

	case WM_DESTROY:
		KillTimer(hwnd, ID_TIMER_SECONDS_TICK) ;
		KillTimer(hwnd, ID_TIMER_HIDE_CFG_PANEL);
		PostQuitMessage (0) ;
		return 0 ;
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

INT_PTR CALLBACK 
Dlgproc_CountdownCfg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndMain = GetParent(hDlg);

	switch (message)
	{{
	case WM_INITDIALOG :
	{
//		vaDbg(_T("WM_INITDIALOG wParam=%X"), wParam);

		const TCHAR *pszCfg = Seconds_to_HMS(g_seconds_countdown_cfg);
		SetDlgItemText(hDlg, IDC_EDIT1, pszCfg);

		HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
		Editbox_EnableUpDownKeyAdjustNumber(hEdit, 0, 59, true, true);

		// Place editbox caret at end, bcz when debugging, we fiddle with seconds often.
		int textlen = GetWindowTextLength (hEdit);
		Edit_SetSel(hEdit, textlen, textlen);
			
		// Specify focus to the editbox, explicitly.
		SetFocus(hEdit);
		return FALSE; // FALSE to disobey dialog manager's suggested focus(would be IDC_EDIT1).
	}
	case WM_COMMAND:
	{
		int idcmd = GET_WM_COMMAND_ID(wParam,lParam);
//		vaDbg(_T("Parent is 0x%X, idcmd=%d"), (UINT)hwndMain, idcmd);
		
		if(idcmd==IDOK)
		{
			TCHAR szHMS[20] = {};
			GetDlgItemText(hDlg, IDC_EDIT1, szHMS, ARRAYSIZE(szHMS)-1);
			int seconds = HMS_to_Seconds(szHMS);
			if(seconds<0)
				return TRUE;

			g_seconds_countdown_cfg = seconds;
			g_msectick_start = GetTickCount();

			g_seconds_remain = 1; // arbitrary >0 value
			DoTimer(hwndMain, ID_TIMER_SECONDS_TICK);

			GetCursorPos(&g_ptClickCountDown);
			ShowWindow(hDlg, SW_HIDE);

			InvalidateRect(hwndMain, NULL, TRUE);			
		}

		return TRUE;
	}
	
	default:
		break;
	}}
	return FALSE ;
}
