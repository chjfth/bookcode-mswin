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
int g_cItems = 10;    // count of color stripes
int g_cyItem = 20;    // height of a color stripe
int g_cxItem = 200;   // width  of a color stripe

HWND g_hwndTT;
int g_iItemTip;

BOOL 
myGetItemRect(int iItem, RECT *prc)
{
	SetRect(prc, 
		0,        g_cyItem * iItem,
		g_cxItem, g_cyItem * (iItem + 1)
		);
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
	ti.lpszText = LPSTR_TEXTCALLBACK; // new
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

LRESULT
OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	if (pnm->hwndFrom == g_hwndTT) 
	{
		switch (pnm->code) {
		case (TTN_NEEDTEXT|TTN_GETDISPINFO): // same value
			{
				// Chj memo: g_iItemTip is updated in every WM_MOUSEMOVE.

				NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnm;
				if (g_iItemTip >= 0) {
					// szText is 80 characters, so %d will fit
					wsprintf(pdi->szText, TEXT("g_iItemTip=%d"), g_iItemTip);
				} else {
					pdi->szText[0] = TEXT('\0');
				}
				pdi->lpszText = pdi->szText;
			}
			break;
		}
	}
	return 0;
}


void
myInvalidateItems(HWND hwnd, int iItemMin, int iItemMax)
{
	RECT rc;
	SetRect(&rc, 0, g_cyItem * iItemMin,
		g_cxItem, g_cyItem * iItemMax);
	InvalidateRect(hwnd, &rc, TRUE);
}

void
myUpdateTooltipFromMessagePos(HWND hwnd)
{
	DWORD dwPos = GetMessagePos();
	POINT pt = { GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos) };
	
	ScreenToClient(hwnd, &pt);
	myUpdateTooltip(pt.x, pt.y);
}

void
OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	switch (ch) {
	case TEXT('+'):
		g_cItems += cRepeat;
		myInvalidateItems(hwnd, g_cItems - cRepeat, g_cItems);
		myUpdateTooltipFromMessagePos(hwnd);
		break;
	case TEXT('-'):
		if (cRepeat > g_cItems) 
			cRepeat = g_cItems;
		g_cItems -= cRepeat;
		myInvalidateItems(hwnd, g_cItems, g_cItems + cRepeat);
		myUpdateTooltipFromMessagePos(hwnd);
		break;
	}
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

	HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify); // new
	HANDLE_MSG(hwnd, WM_CHAR, OnChar); // new

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
	wc.lpszClassName = TEXT("DynamicTextTooltip");
	
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
			TEXT("DynamicTextTooltip"),                /* Class Name */
			TEXT("DynamicTextTooltip"),                /* Title */
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

