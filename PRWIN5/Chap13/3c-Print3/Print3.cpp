/*---------------------------------------
   PRINT3.C -- Printing with Dialog Box
               (c) Charles Petzold, 1998
  ---------------------------------------*/

#include <windows.h>
#include "..\..\vaDbg.h"

HDC  GetPrinterDC (void) ;              // in GETPRNDC.C
void PageGDICalls (HDC, int, int) ;     // in PRINT.C

HINSTANCE hInst ;
TCHAR     szAppName[] = TEXT ("Print3") ;
TCHAR     szCaption[] = TEXT ("Print Program 3 (Dialog Box)") ;

BOOL g_bUserAbort ;
HWND g_hDlgPrint ;

BOOL CALLBACK PrintDlgProc (HWND hDlg, UINT message, 
	WPARAM wParam, LPARAM lParam)
{
	TCHAR sznow[40];
	now_timestr(sznow, ARRAYSIZE(sznow));
	vaDbg(_T("%s PrintDlgProc, message=%d(0x%X)"), sznow, message, message);

	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText (hDlg, szAppName) ;
		EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED) ;
		return TRUE ;

	case WM_COMMAND:
		g_bUserAbort = TRUE ;
		EnableWindow (GetParent (hDlg), TRUE) ;
		DestroyWindow (hDlg) ;
		g_hDlgPrint = NULL ;
		return TRUE ;
	}
	return FALSE ;
}

BOOL CALLBACK AbortProc (HDC hdcPrn, int iError)
{
	TCHAR sznow[40];
	now_timestr(sznow, ARRAYSIZE(sznow));
	vaDbg(_T("%s AbortProc, iError=%d"), sznow, iError);

	MSG msg ;

	while (!g_bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!g_hDlgPrint || !IsDialogMessage (g_hDlgPrint, &msg))
		{
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ;
		}
	}
	return !g_bUserAbort ;
}

BOOL PrintMyPage (HWND hwnd)
{
	static DOCINFO di = { sizeof (DOCINFO), TEXT ("Print3: Printing") } ;
	BOOL           bSuccess = TRUE ;
	HDC            hdcPrn ;
	int            xPage, yPage ;

	if (NULL == (hdcPrn = GetPrinterDC ()))
		return FALSE ;

	xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
	yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

	EnableWindow (hwnd, FALSE) ;

	g_bUserAbort = FALSE ;
	g_hDlgPrint = CreateDialog (hInst, TEXT ("PrintDlgBox"), 
		hwnd, PrintDlgProc) ;

	SetAbortProc (hdcPrn, AbortProc) ;

	if (StartDoc (hdcPrn, &di) > 0)
	{
		if (StartPage (hdcPrn) > 0)
		{
			PageGDICalls (hdcPrn, xPage, yPage) ;

			if (EndPage (hdcPrn) > 0)
				EndDoc (hdcPrn) ;
			else
				bSuccess = FALSE ;
		}
	}
	else
		bSuccess = FALSE ;

	if (!g_bUserAbort)
	{
		EnableWindow (hwnd, TRUE) ;
		DestroyWindow (g_hDlgPrint) ;
	}

	DeleteDC (hdcPrn) ;

	return bSuccess && !g_bUserAbort ;
}
