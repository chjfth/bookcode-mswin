/*-------------------------------------------------------
   POPPAD1.C -- Popup Editor using child window edit box
                (c) Charles Petzold, 1998
  -------------------------------------------------------*/

#include <windows.h>
#include <windowsX.h>


//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define ID_EDIT     1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT ("PopPad1") ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	HWND     hwnd ;
	MSG      msg ;
	WNDCLASS wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon(hInstance, TEXT("MYPROGRAM"));
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName, szAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		600, 500, // CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL) ;

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ; 

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return (int)msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit ;

	switch (message)
	{
	case WM_CREATE :
		hwndEdit = CreateWindow (TEXT ("edit"), NULL,
			WS_CHILD | WS_VISIBLE | 
			WS_HSCROLL | WS_VSCROLL |
			WS_BORDER | 
			ES_LEFT | 
			ES_MULTILINE |
			ES_AUTOHSCROLL | ES_AUTOVSCROLL
			,
			0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
			((LPCREATESTRUCT) lParam) -> hInstance, NULL) ;

		// Chj special: Want fixed-width font in the editbox.
		SetWindowFont(hwndEdit, 
			GetStockFont(OEM_FIXED_FONT), // or SYSTEM_FIXED_FONT
			TRUE);

		return 0 ;

	case WM_SETFOCUS :
		SetFocus (hwndEdit) ;
		return 0 ;

	case WM_SIZE : 
		MoveWindow (hwndEdit, 0, 0, LOWORD (lParam), HIWORD (lParam), TRUE) ;
		return 0 ;

	case WM_COMMAND :
		if (LOWORD (wParam) == ID_EDIT)
		{
			if (HIWORD(wParam)==EN_ERRSPACE)
			{
				MessageBox (hwnd, TEXT ("Edit control out of space. (EN_ERRSPACE)"),
					szAppName, MB_OK | MB_ICONSTOP) ;
			}
			if (HIWORD(wParam)==EN_MAXTEXT)
			{
				MessageBox (hwnd, TEXT ("Edit control out of space. (EN_MAXTEXT)"),
					szAppName, MB_OK | MB_ICONSTOP) ;
			}
		}
		return 0 ;

	case WM_DESTROY :
		PostQuitMessage (0) ;
		return 0 ;
	}
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
