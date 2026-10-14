#ifndef __CxxRawWinBase_h_
#define __CxxRawWinBase_h_

#include <assert.h>
#include <tchar.h>
#include <windows.h>

class CxxRawWinBase
{
protected:
	HWND m_hwnd;

public:
	virtual ~CxxRawWinBase() {}

	CxxRawWinBase() 
	{ 
		m_hwnd = NULL;
	}

	HWND Hwnd() { return m_hwnd; }

public:
	
	// User can override this to customize register-class and window-create.
	virtual BOOL RegisterAndCreate(const TCHAR *szAppName)
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);

		WNDCLASS     wndclass = {};

		wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
		wndclass.lpfnWndProc   = s_WndProc ; // MUST preserve this
		wndclass.cbClsExtra    = 0 ;
		wndclass.cbWndExtra    = 0 ;
		wndclass.hInstance     = hInstance;
		wndclass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(1));
		wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
		wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
		wndclass.lpszMenuName  = NULL ;
		wndclass.lpszClassName = szAppName ;

		RegisterClass (&wndclass);

		DWORD exstyle = 0;

		m_hwnd = CreateWindowEx (exstyle,
			szAppName,    // window class name
			szAppName,            // window caption
			WS_OVERLAPPEDWINDOW,  // window style
			CW_USEDEFAULT,   // initial x position
			CW_USEDEFAULT,   // initial y position
			400,             // initial x size
			200,             // initial y size
			NULL,            // parent window handle
			NULL,            // window menu handle
			hInstance,       // program instance handle
			this     // MUST preserve this, as LPCREATESTRUCT to WM_NCCREATE
			);
		if(!m_hwnd)
			return FALSE;

		ShowWindow (m_hwnd, SW_NORMAL) ;
		UpdateWindow (m_hwnd) ;

		return TRUE;
	}
	
	// User should override this to do window message processing.
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		assert(m_hwnd);
		assert(hwnd==m_hwnd);

		if(uMsg==WM_DESTROY)
		{
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc (hwnd, uMsg, wParam, lParam) ;
	}


protected:
	// Derived class should always use s_WndProc as wndclass.lpfnWndProc.
	// Customization is done by overriding the public virtual WndProc().
	static LRESULT CALLBACK s_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CxxRawWinBase *pwnd = NULL;

		if(uMsg==WM_NCCREATE)
		{
			pwnd = (CxxRawWinBase*)((CREATESTRUCT*)lParam)->lpCreateParams;
			assert(pwnd);

			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pwnd);

			pwnd->m_hwnd = hwnd;
		}
		else
		{
			pwnd = (CxxRawWinBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		LRESULT lret = 0;

		if(pwnd)
		{
			lret = pwnd->WndProc(hwnd, uMsg, wParam, lParam);
		}
		else
		{
			assert(uMsg==WM_GETMINMAXINFO);
		}

		return lret;
	}
};

#endif
