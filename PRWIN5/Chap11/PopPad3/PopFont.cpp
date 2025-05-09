/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>

static LOGFONT g_logfont ;
static HFONT   g_hFont ;

BOOL PopFontChooseFont (HWND hwnd)
{
	CHOOSEFONT cf = {0};

	cf.lStructSize    = sizeof (CHOOSEFONT) ;
	cf.hwndOwner      = hwnd ;
	cf.hDC            = NULL ;
	cf.lpLogFont      = &g_logfont ;
	cf.iPointSize     = 0 ;
	cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS ;
	cf.rgbColors      = 0 ;
	cf.lCustData      = 0 ;
	cf.lpfnHook       = NULL ;
	cf.lpTemplateName = NULL ;
	cf.hInstance      = NULL ;
	cf.lpszStyle      = NULL ;
	cf.nFontType      = 0 ;               // Returned from ChooseFont
	cf.nSizeMin       = 0 ;
	cf.nSizeMax       = 0 ;

	return ChooseFont (&cf) ;
}

void PopFontInitialize (HWND hwndEdit)
{
	// Chj: Better use a fixed-width font for the editbox.
	g_hFont = CreateFont (0, 0, 0, 0, 0, 0, 0, 0,
		DEFAULT_CHARSET, 
		0, 0, 0, FIXED_PITCH, NULL); 

	SendMessage (hwndEdit, WM_SETFONT, (WPARAM) g_hFont, 0) ;
}

void PopFontSetFont (HWND hwndEdit)
{
	HFONT hFontNew ;
	RECT  rect ;

	hFontNew = CreateFontIndirect (&g_logfont) ;
	SendMessage (hwndEdit, WM_SETFONT, (WPARAM) hFontNew, 0) ;
	DeleteObject (g_hFont) ;
	g_hFont = hFontNew ;
	GetClientRect (hwndEdit, &rect) ;
	InvalidateRect (hwndEdit, &rect, TRUE) ;
}

void PopFontDeinitialize (void)
{
	DeleteObject (g_hFont) ;
}
