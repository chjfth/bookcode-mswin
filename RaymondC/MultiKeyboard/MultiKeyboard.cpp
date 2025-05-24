#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <strsafe.h>

// [2024-11-08] RaymondC article:
// https://devblogs.microsoft.com/oldnewthing/20160627-00/?p=93755
// If I have multiple attached keyboards, how can I read input from each one individually?

HINSTANCE g_hinst;          /* This application's HINSTANCE */
HWND g_hwndChild;           /* Optional child window */

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
	//// This Program Special:

	g_hwndChild = CreateWindow(TEXT("listbox"), NULL,
		LBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
		0, 0, 0, 0, hwnd, NULL, g_hinst, 0);

	RAWINPUTDEVICE dev = {};
	dev.usUsagePage = 1;
	dev.usUsage = 6;
	dev.dwFlags = 0;
	dev.hwndTarget = hwnd;
	RegisterRawInputDevices(&dev, 1, sizeof(dev));

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
	//// This Program Special:

	RAWINPUTDEVICE dev = {};
	dev.usUsagePage = 1;
	dev.usUsage = 6;
	dev.dwFlags = RIDEV_REMOVE;
	dev.hwndTarget = hwnd;
	RegisterRawInputDevices(&dev, 1, sizeof(dev));

	PostQuitMessage(0);
}

//// This Program Special:

#define HANDLE_WM_INPUT(hwnd, wParam, lParam, fn) \
	((fn)((hwnd), GET_RAWINPUT_CODE_WPARAM(wParam), (HRAWINPUT)(lParam)), 0)


void OnInput(HWND hwnd, WPARAM code, HRAWINPUT hRawInput)
{
	UINT dwSize = 0;
	GetRawInputData(hRawInput, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT *rawinput = (RAWINPUT *)malloc(dwSize);
	GetRawInputData(hRawInput, RID_INPUT, rawinput, &dwSize, sizeof(RAWINPUTHEADER));

	if (rawinput->header.dwType == RIM_TYPEKEYBOARD) 
	{
		TCHAR prefix[80] = {};
		if (rawinput->data.keyboard.Flags & RI_KEY_E0) 
		{
			StringCchCat(prefix, ARRAYSIZE(prefix), TEXT("E0 "));
		}
		if (rawinput->data.keyboard.Flags & RI_KEY_E1) 
		{
			StringCchCat(prefix, ARRAYSIZE(prefix), TEXT("E1 "));
		}

		static int si = 0;
		TCHAR buffer[256] = {};
		StringCchPrintf(buffer, ARRAYSIZE(buffer),
			TEXT("[#%d] %p, msg=%04x, vk=%04x, scanCode=%s%02x, %s"),
			++si,
			rawinput->header.hDevice,
			// msg=
			rawinput->data.keyboard.Message,
			rawinput->data.keyboard.VKey,
			// scanCode=...
			prefix,
			rawinput->data.keyboard.MakeCode,
			// key-press or key-release?
			(rawinput->data.keyboard.Flags & RI_KEY_BREAK)	? TEXT("release") : TEXT("press"));
		
		ListBox_AddString(g_hwndChild, buffer);

		// Scroll to final listbox item
		int cnt = ListBox_GetCount(g_hwndChild);
		ListBox_SetCurSel(g_hwndChild, cnt-1); 
	}

	DefRawInputProc(&rawinput, 1, sizeof(RAWINPUTHEADER));
	free(rawinput);
}


/*
 *  PaintContent
 *      Interesting things will be painted here eventually.
 */
void
PaintContent(HWND hwnd, PAINTSTRUCT *pps)
{
}

/*
 *  OnPaint
 *      Paint the content as part of the paint cycle.
 */
void
OnPaint(HWND hwnd)
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
void
OnPrintClient(HWND hwnd, HDC hdc)
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
	HANDLE_MSG(hwnd, WM_INPUT, OnInput);

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
	wc.lpszClassName = TEXT("MultiKeyboard");
	
	if (!RegisterClass(&wc)) 
		return FALSE;
	
	InitCommonControls();               /* In case we use a common control */
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev,
				   LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg;
	HWND hwnd;
	g_hinst = hinst;

	if (!InitApp()) 
		return 0;

	if (SUCCEEDED(CoInitialize(NULL))) {/* In case we use COM */
		hwnd = CreateWindow(
			TEXT("MultiKeyboard"),                /* Class Name */
			TEXT("MultiKeyboard"),                /* Title */
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

