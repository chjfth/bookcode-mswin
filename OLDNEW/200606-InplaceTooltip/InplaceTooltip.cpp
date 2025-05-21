#define STRICT
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <commctrl.h>
#include <shlwapi.h>

// Coding in-place tooltips
// https://devblogs.microsoft.com/oldnewthing/20060626-11/?p=30743


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// -- [2025-05-21] Without this on Win7, TTM_ADDTOOL will fail.


HINSTANCE g_hinst;          /* This application's HINSTANCE */
HWND g_hwndChild;           /* Optional child window */

// new >>>
HFONT g_hfTT;
HWND g_hwndTT;
RECT g_rcText;
LPCTSTR g_pszText = TEXT("Lorem ipsum dolor sit amet.");
const int c_xText = 50;
const int c_yText = 50;
// new <<<

/*
 *  OnSize
 *      If we have an inner child, resize it to fit.
 */
void
OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if (g_hwndChild) {
		MoveWindow(g_hwndChild, 0, 0, cx, cy, TRUE);
	}
}

/*
 *  OnCreate
 *      Applications will typically override this and maybe even
 *      create a child window.
 */
BOOL
OnCreate(HWND hwnd, LPCREATESTRUCT lpcs)
{
	g_hwndTT = CreateWindowEx(WS_EX_TRANSPARENT, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		TTS_NOPREFIX | TTS_NOANIMATE,
		0, 0, 0, 0,
		hwnd, NULL, g_hinst, NULL);
	if (!g_hwndTT)
		return FALSE;

	g_hfTT = GetStockFont(ANSI_VAR_FONT);
	SetWindowFont(g_hwndTT, g_hfTT, FALSE);
	HDC hdc = GetDC(hwnd);
	HFONT hfPrev = SelectFont(hdc, g_hfTT);
	SIZE siz;
	GetTextExtentPoint(hdc, g_pszText, lstrlen(g_pszText), &siz);
	SetRect(&g_rcText, c_xText, c_yText,
		c_xText + siz.cx, c_yText + siz.cy);
	SelectFont(hdc, hfPrev);
	ReleaseDC(hwnd, hdc);

	TOOLINFO ti = { sizeof(ti) };
	ti.uFlags = TTF_TRANSPARENT | TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.uId = 0;
	ti.lpszText = const_cast<LPTSTR>(g_pszText);
	ti.rect = g_rcText;

	BOOL succ = SendMessage(g_hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);
	return TRUE;
}

/*
 *  OnDestroy
 *      Post a quit message because our application is over when the
 *      user closes this window.
 */
void
OnDestroy(HWND hwnd)
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
	HFONT hfPrev = SelectFont(pps->hdc, g_hfTT);
	TextOut(pps->hdc, g_rcText.left, g_rcText.top,
		g_pszText, lstrlen(g_pszText));
	SelectFont(pps->hdc, hfPrev);
}

/*
 *  OnPaint
 *      Paint the content as part of the paint cycle.
 */
void
OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {};
	BeginPaint(hwnd, &ps);
	PaintContent(hwnd, &ps);
	EndPaint(hwnd, &ps);
}

/*
 *  OnPrintClient
 *      Paint the content as requested by USER.
 */
void
OnPrintClient(HWND hwnd, HDC hdc)
{
	PAINTSTRUCT ps = {};
	ps.hdc = hdc;
	GetClientRect(hwnd, &ps.rcPaint);
	PaintContent(hwnd, &ps);
}

// new >>>

LRESULT OnTooltipShow(HWND hwnd, NMHDR *pnm)
{
	RECT rc = g_rcText;
	MapWindowRect(hwnd, NULL, &rc);
	SendMessage(pnm->hwndFrom, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc);
	SetWindowPos(pnm->hwndFrom, 0, rc.left, rc.top, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	return TRUE; // suppress default positioning
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	if (pnm->hwndFrom == g_hwndTT) {
		switch (pnm->code) {
		case TTN_SHOW:
			return OnTooltipShow(hwnd, pnm);
		}
	}
	return 0;
}

// new <<<


/*
 *  Window procedure
 */
LRESULT CALLBACK
WndProc(HWND hwnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg) 
	{{
	HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
	HANDLE_MSG(hwnd, WM_SIZE, OnSize);
	HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
	HANDLE_MSG(hwnd, WM_PAINT, OnPaint);

	HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);

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
	wc.lpszClassName = TEXT("InplaceTooltip");
	
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
			TEXT("InplaceTooltip"),         /* Class Name */
			TEXT("Raymond Inplace-Tooltip"), /* Title */
			WS_OVERLAPPEDWINDOW,            /* Style */
			CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
			400, 200,                       /* Size */
			NULL,                           /* Parent */
			NULL,                           /* No menu */
			hinst,                          /* Instance */
			0);                             /* No special parameters */
		assert(hwnd);

		ShowWindow(hwnd, nShowCmd);

		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		CoUninitialize();
	}
	return 0;
}

