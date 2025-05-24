#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <commctrl.h>
#include <shlwapi.h>

// Multiplexing multiple tools into one in a tooltip
// https://devblogs.microsoft.com/oldnewthing/20060628-05/?p=30703

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinst;          /* This application's HINSTANCE */
HWND g_hwndChild;           /* Optional child window */

// new:
int g_cItems = 10;
int g_cyItem = 20;
int g_cxItem = 200;

HWND g_hwndTT;
int g_iItemTip;

BOOL 
myGetItemRect(int iItem, RECT *prc)
{
	SetRect(prc, 0, g_cyItem * iItem,
		g_cxItem, g_cyItem * (iItem + 1));
	return iItem >= 0 && iItem < g_cItems;
}

int 
myItemHitTest(int x, int y)
{
	if (x < 0 || x > g_cxItem) 
		return -1;
	
	if (y < 0 || y > g_cItems * g_cyItem) 
		return -1;
	
	return y / g_cyItem;
}

void
myUpdateTooltip(int mousex, int mousey)
{
	int iItemOld = g_iItemTip;
	g_iItemTip = myItemHitTest(mousex, mousey);

	if (iItemOld != g_iItemTip) {
		SendMessage(g_hwndTT, TTM_POP, 0, 0); // hide the tooltip
	}
}

/*
 *  OnSize
 *      If we have an inner child, resize it to fit.
 */
void 
OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	// Chj: Make the hottool-area always equal to whole client-area.

	TOOLINFO ti = { sizeof(ti) };
	ti.hwnd = hwnd;
	ti.uId = 0;
	GetClientRect(hwnd, &ti.rect);
	SendMessage(g_hwndTT, TTM_NEWTOOLRECT, 0, (LPARAM)&ti);
}


/*
 *  OnCreate
 *      Applications will typically override this and maybe even
 *      create a child window.
 */
BOOL 
OnCreate(HWND hwnd, LPCREATESTRUCT lpcs)
{
	g_hwndTT = CreateWindowEx(WS_EX_TRANSPARENT, TOOLTIPS_CLASS, NULL,
		TTS_NOPREFIX,
		0, 0, 0, 0,
		hwnd, NULL, g_hinst, NULL);

	if (!g_hwndTT) 
		return FALSE;
	
	g_iItemTip = -1;
	TOOLINFO ti = { sizeof(ti) };
	ti.uFlags = TTF_TRANSPARENT;
	ti.hwnd = hwnd;
	ti.uId = 0;
	ti.lpszText = TEXT("Placeholder tooltip");
//	SetRectEmpty(&ti.rect); // no need
	SendMessage(g_hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	return TRUE;
}

/*
 *  OnDestroy
 *      Post a quit message because our application is over when the
 *      user closes this window.
 */
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}

/*
 *  PaintContent
 *      Interesting things will be painted here eventually.
 */
void
PaintContent(HWND hwnd, PAINTSTRUCT *pps)
{
	COLORREF clrSave = GetBkColor(pps->hdc);
	for (int iItem = 0; iItem < g_cItems; iItem++) 
	{
		RECT rc = {};
		myGetItemRect(iItem, &rc);
		COLORREF clr = RGB(
			(iItem & 1) ? 0x7F : 0,
			(iItem & 2) ? 0x7F : 0,
			(iItem & 4) ? 0x7F : 0);
	
		if (iItem & 8) 
			clr *= 2;
		
		SetBkColor(pps->hdc, clr);

		ExtTextOut(pps->hdc, rc.left, rc.top,
			ETO_OPAQUE, &rc, TEXT(""), 0, NULL);
	}
	SetBkColor(pps->hdc, clrSave);}

/*
 *  OnPaint
 *      Paint the content as part of the paint cycle.
 */
void OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	PaintContent(hwnd, &ps);
	EndPaint(hwnd, &ps);
}

/*
 *  OnPrintClient
 *      Paint the content as requested by USER.
 */
void OnPrintClient(HWND hwnd, HDC hdc)
{
	PAINTSTRUCT ps;
	ps.hdc = hdc;
	GetClientRect(hwnd, &ps.rcPaint);
	PaintContent(hwnd, &ps);
}

void
myRelayEvent(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	myUpdateTooltip(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	
	MSG msg = {};
	msg.hwnd = hwnd;
	msg.message = uiMsg;
	msg.wParam = wParam;
	msg.lParam = lParam;
	SendMessage(g_hwndTT, TTM_RELAYEVENT, 0, (LPARAM)&msg);
}

/*
 *  Window procedure
 */
LRESULT CALLBACK
WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	// Important: Do TTM_RELAYEVENT to tooltip-window.
	if ((uiMsg >= WM_MOUSEFIRST && uiMsg <= WM_MOUSELAST) 
		|| uiMsg == WM_NCMOUSEMOVE) 
	{
		myRelayEvent(hwnd, uiMsg, wParam, lParam);
	}

	switch (uiMsg) 
	{{
	HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
	HANDLE_MSG(hwnd, WM_SIZE, OnSize);
	HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
	HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
	case WM_PRINTCLIENT: 
	{
		OnPrintClient(hwnd, (HDC)wParam); 
		return 0;
	}
	}}
	return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

BOOL
InitApp(void)
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hinst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("MultiplexTooltip");
	
	if (!RegisterClass(&wc)) 
		return FALSE;
	
	InitCommonControls();               /* In case we use a common control */
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev,
				   LPSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	MSG msg;
	HWND hwnd;
	g_hinst = hinst;

	if (!InitApp()) 
		return 0;

	if (SUCCEEDED(CoInitialize(NULL))) /* In case we use COM */
	{
		hwnd = CreateWindow(
			TEXT("MultiplexTooltip"),                /* Class Name */
			TEXT("MultiplexTooltip"),                /* Title */
			WS_OVERLAPPEDWINDOW,            /* Style */
			CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
			400, 300,                       /* Size */
			NULL,                           /* Parent */
			NULL,                           /* No menu */
			hinst,                          /* Instance */
			0);                             /* No special parameters */

		ShowWindow(hwnd, nShowCmd);

		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		CoUninitialize();
	}
	return 0;
}

