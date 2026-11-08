// https://devblogs.microsoft.com/oldnewthing/20160629-00/?p=93775/
// Raymond did not actually run the code, so many typo in his original blog post.
// Now I have fixed them.

#define STRICT
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <commctrl.h>
#include <shlwapi.h>

/*
[2025-05-24] Chj coding hint: If I add/change a function in concrete project, 
I will leave that function's return type on its dedicated line, so to imply
that this function contains specific content for this specific project.
*/

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "RuntimeObject.lib")

#define CHHI_ALL_IMPL
#include <mswin/utils_wingui.h>

const TCHAR *MAINWND_CLASSNAME = _T("DetectGameBar");

HINSTANCE g_hinst;          /* This application's HINSTANCE */
HWND g_hwndChild;           /* Optional child window */

#include <wrl/client.h> 
#include <wrl/event.h> 
#include <wrl/wrappers/corewrappers.h> 
#include <windows.gaming.ui.h> 
#include <EventToken.h> 
#include <tchar.h> // Huh? Why are you still using ANSI? 

namespace WRL = Microsoft::WRL; 
namespace awf = ABI::Windows::Foundation; 
namespace gameui = ABI::Windows::Gaming::UI; 

WRL::ComPtr<gameui::IGameBarStatics> g_gameBarStatics; 
boolean g_isVisible; boolean g_isInputRedirected; 
EventRegistrationToken g_tokenVisibility; 
EventRegistrationToken g_tokenInput;

void CheckGameBarVisibility(HWND hwnd)
{
	boolean isVisible;
	g_gameBarStatics->get_Visible(&isVisible);
	if (g_isVisible != isVisible)
	{
		g_isVisible = isVisible;
		InvalidateRect(hwnd, nullptr, TRUE);
	}
}

void CheckGameBarInput(HWND hwnd)
{
	boolean isInputRedirected;
	g_gameBarStatics->get_IsInputRedirected(&isInputRedirected);
	if (g_isInputRedirected != isInputRedirected)
	{
		g_isInputRedirected = isInputRedirected;
		InvalidateRect(hwnd, nullptr, TRUE);
	}
}


/*
 *  OnSize
 *      If we have an inner child, resize it to fit.
 */
void OnSize(HWND hwnd, UINT state, int cx, int cy)
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
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpcs)
{
	util_SetWindowIcon(hwnd, MAKEINTRESOURCE(1));

	Windows::Foundation::GetActivationFactory(WRL::Wrappers::HStringReference(
		RuntimeClass_Windows_Gaming_UI_GameBar).Get(), &g_gameBarStatics);

	auto visibilityHandler = WRL::Callback<awf::IEventHandler<IInspectable*>>(
		[hwnd](IInspectable*, IInspectable*)
	{
		CheckGameBarVisibility(hwnd);
		return S_OK;
	});
	g_gameBarStatics->add_VisibilityChanged(visibilityHandler.Get(), &g_tokenVisibility);

	auto inputHandler = WRL::Callback<awf::IEventHandler<IInspectable*>>(
		[hwnd](IInspectable*, IInspectable*)
	{
		CheckGameBarInput(hwnd);
		return S_OK;
	});
	g_gameBarStatics->add_IsInputRedirectedChanged(inputHandler.Get(), &g_tokenInput);

	CheckGameBarVisibility(hwnd);
	CheckGameBarInput(hwnd);
	return TRUE;
}

/*
 *  OnDestroy
 *      Post a quit message because our application is over when the
 *      user closes this window.
 */
void OnDestroy(HWND hwnd)
{
	g_gameBarStatics->remove_VisibilityChanged(g_tokenVisibility);
	g_gameBarStatics->remove_IsInputRedirectedChanged(g_tokenInput);
	g_gameBarStatics.Reset();

	PostQuitMessage(0);
}

/*
 *  PaintContent
 *      Interesting things will be painted here eventually.
 */
void PaintContent(HWND hwnd, PAINTSTRUCT *pps)
{
	PCTSTR visibleMessage =
		g_isVisible ? TEXT("GameBar is visible")
		: TEXT("GameBar is not visible");

	TextOut(pps->hdc, 0, 0, visibleMessage, (int)_tcslen(visibleMessage));

	PCTSTR inputMessage =
		g_isInputRedirected ? TEXT("GameBar has taken input")
		: TEXT("GameBar does not have input");

	TextOut(pps->hdc, 0, 20, inputMessage, (int)_tcslen(inputMessage));
}

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
	case WM_PRINTCLIENT: 
	{
		OnPrintClient(hwnd, (HDC)wParam); 
		return 0;
	}
	}}
	return DefWindowProc(hwnd, uiMsg, wParam, lParam);
}

BOOL InitApp(void)
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
	wc.lpszClassName = MAINWND_CLASSNAME;
	
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
			MAINWND_CLASSNAME,              /* Class Name */
			TEXT("DetectGameBar program"),        /* Title */
			WS_OVERLAPPEDWINDOW,            /* Style */
			CW_USEDEFAULT, CW_USEDEFAULT,   /* Position */
			500, 400,                       /* Size */
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

