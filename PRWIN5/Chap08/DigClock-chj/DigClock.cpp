/*-----------------------------------------
DIGCLOCK.c -- Digital Clock(c) Charles Petzold, 1998

DigClock.cpp -- Updated by Chj, 2021.11.
* Thin window border.
* Drag window by clicking anywhere inside the window.
* Move window by pressing arrow keys, Ctrl to accelerate.
* Right-click context menu, toggle always on top.
* Select different color by mouse clicking.
* Scale down "second" value display(75%), so to have it stand out.
* Eliminate UI flickering by MemDC-double-buffering.
  -----------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include "resource.h"

#include "..\..\vaDbg.h"
#include "..\..\BeginPaint_NoFlicker.h"

void ShowHelp(HWND hwndParent)
{
	static TCHAR *s_help =
		_T("DigClock from Charles Petzold [PRWIN5] Chap08, with improvements from Chj.\r\n")
		_T("\r\n")
		_T("To Move the clock window:\r\n")
		_T("(1) Click and drag with mouse left button.\r\n")
		_T("(2) Use keyboard arrow keys, pixel by pixel. Press Ctrl key to accelerate.\r\n")
		_T("\r\n")
		_T("To change digit color: \r\n")
		_T("(1) Left click on the clock for next color.\r\n")
		_T("(2) Ctrl + Left-click to cycle back.\r\n")
		;
	MessageBox(hwndParent, s_help, _T("Help"), MB_OK);
}

#define ID_TIMER    1

SIZE g_init_winsize = {160, 60};

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("DigClock") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

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

	hwnd = CreateWindowEx (WS_EX_DLGMODALFRAME,
		szAppName, TEXT ("Digital Clock"),
		WS_POPUPWINDOW|WS_THICKFRAME,  // WS_OVERLAPPEDWINDOW,
		mousepos.x, mousepos.y,
		g_init_winsize.cx, g_init_winsize.cy, // CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, NULL, hInstance, NULL) ;

	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance, _T("MYPROGRAM")));

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

static COLORREF s_colors[] = 
{
	RGB(0x54, 0x84, 0x14), // swamp green
	RGB(0x30, 0xA0, 0xD0), // dyeing blue
	RGB(0xD4, 0x62, 0x62), // dark red
	RGB(0xC0, 0x30, 0xFF), // shining purple
	RGB(0x70, 0x42, 0x14), // brown
	RGB(0xF8, 0x60, 0x30), // deep orange
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
	// digit display size to 75% to make the second value stands out.
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
	if (!fSuppress || (iNumber / 10 != 0))
		DisplayDigit (hdc, iNumber / 10, is_scale_down) ;

	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
	
	DisplayDigit (hdc, iNumber % 10, is_scale_down) ;
	OffsetWindowOrgEx (hdc, -42, 0, NULL) ;
}

void DisplayColon (HDC hdc)
{
	POINT ptColon [2][4] = { 2,  21,  6,  17,  10, 21,  6, 25,
		2,  51,  6,  47,  10, 51,  6, 55 } ;

	Polygon (hdc, ptColon [0], 4) ;
	Polygon (hdc, ptColon [1], 4) ;

	OffsetWindowOrgEx (hdc, -12, 0, NULL) ;
}

void DisplayTime (HDC hdc, BOOL f24Hour, BOOL fSuppress)
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

static BOOL   f24Hour, fSuppress ;
//
void ReloadSetting(HWND hwnd)
{
	TCHAR         szBuffer [2] ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2) ;
	f24Hour = (szBuffer[0] == '1') ;

	GetLocaleInfo (LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2) ;
	fSuppress = (szBuffer[0] == '0') ;

	InvalidateRect (hwnd, NULL, TRUE) ;
}

void MoveWindow_byOffset(HWND hwnd, int offsetx, int offsety)
{
	RECT oldrect = {};
	GetWindowRect(hwnd, &oldrect);
	MoveWindow(hwnd, oldrect.left+offsetx, oldrect.top+offsety, 
		oldrect.right-oldrect.left, oldrect.bottom-oldrect.top, TRUE);
}

void Hwnd_SetAlwaysOnTop(HWND hwnd, bool istop)
{
	SetWindowPos(hwnd, 
		istop? HWND_TOPMOST : HWND_NOTOPMOST,
		0,0,0,0, SWP_NOMOVE|SWP_NOSIZE
		);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	static HBRUSH hBrushRed ;
	static int    cxClient, cyClient ;
	HDC           hdc ;
	PAINTSTRUCT   ps ;

	static HMENU s_popmenu;
	static bool s_is_always_on_top = true;

	static POINT s_pos_mousedown; // client-area position
	static bool s_is_dragging = false;
	static bool s_is_moved = false;
	static int s_idxcolor = 0;

	bool isCtrl = GetKeyState(VK_CONTROL)<0;

	switch (message)
	{{
	case WM_CREATE:
	{
		SetTimer (hwnd, ID_TIMER, 1000, NULL) ;

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
		InvalidateRect (hwnd, NULL, TRUE) ;
		return 0 ;

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

		DisplayTime (hdc, f24Hour, fSuppress) ;

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
		if(s_is_dragging==false)
			break;
		
		// Set new window pos

		int offsetx = GET_X_LPARAM(lParam) - s_pos_mousedown.x;
		int offsety = GET_Y_LPARAM(lParam) - s_pos_mousedown.y;
		if(offsetx!=0 || offsety!=0)
		{
			s_is_moved = true;
		}

		MoveWindow_byOffset(hwnd, offsetx, offsety);
		
		return 0; // this return is a must!
	}
	case WM_LBUTTONUP:
	{
		s_is_dragging = false;
		ReleaseCapture();

		if(s_is_moved==false)
		{
			s_idxcolor = Get_NewColorIdx(s_idxcolor, isCtrl ? -1 : 1);
			InvalidateRect(hwnd, NULL, TRUE);
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

		CheckMenuItem(hmenuPopup, IDM_ALWAYS_ON_TOP, 
			s_is_always_on_top ? MF_CHECKED : MF_UNCHECKED);

		return 0;
	}

	case WM_COMMAND:
	{
		int cmdid = GET_WM_COMMAND_ID(wParam, lParam);

		if(cmdid==IDM_ALWAYS_ON_TOP)
		{
			s_is_always_on_top = !s_is_always_on_top;
			Hwnd_SetAlwaysOnTop(hwnd, s_is_always_on_top);
		}
		else if(cmdid==IDM_HELP)
		{
			ShowHelp(hwnd);
		}
		else if(cmdid==IDM_RESET_SIZE)
		{
			RECT oldrect = {};
			GetWindowRect(hwnd, &oldrect);
			MoveWindow(hwnd, oldrect.left, oldrect.top, 
				g_init_winsize.cx, g_init_winsize.cy, TRUE);
		}
		else if(cmdid==IDM_EXIT)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}

		return 0;
	}

	case WM_DESTROY:
		KillTimer (hwnd, ID_TIMER) ;
//		DeleteObject (hBrushRed) ;
		PostQuitMessage (0) ;
		return 0 ;
	}}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
