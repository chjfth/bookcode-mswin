/*----------------------------------------------
   COLORS3.C -- Version using Common Dialog Box
                (c) Charles Petzold, 1998
  ----------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>

#include "resource.h"

HINSTANCE g_exeInst;
HINSTANCE g_dllInst;

UINT_PTR CALLBACK CCHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
	case WM_INITDIALOG:
		
		g_dllInst = GetWindowInstance(hdlg); // (test) it's comdlg32.dll's address
		
		HICON hIcon = LoadIcon(g_exeInst, TEXT("MYPROGRAM"));
		SendMessage(hdlg, WM_SETICON, TRUE, (LPARAM)hIcon);
		break;
	}
	return 0;
}


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	g_exeInst = hInstance;

	static CHOOSECOLOR cc = {};
	static COLORREF    crCustColors[16] ;

	cc.lStructSize    = sizeof (CHOOSECOLOR) ;
	cc.hwndOwner      = NULL ;
	cc.hInstance      = NULL ;
	cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
	cc.lpCustColors   = crCustColors ;
	cc.Flags          = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK ;
	cc.lCustData      = 0 ;
	cc.lpfnHook       = CCHookProc ;
	cc.lpTemplateName = NULL ;

	BOOL succ = ChooseColor (&cc) ;
	return succ ? 0 : 4;
}
