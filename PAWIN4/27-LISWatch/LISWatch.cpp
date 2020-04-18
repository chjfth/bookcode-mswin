/******************************************************************************
Module:  LISWatch.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/


#include "..\CmnHdr.h"     /* See Appendix A. */
#include <tchar.h>
#include <stdio.h>
#include <windowsx.h>
#include "Resource.h"
#include "..\share\dbgprint.h"

///////////////////////////////////////////////////////////////////////////////


#define TIMER_DELAY (500)        // Half a second

UINT_PTR g_uTimerId = 1;
DWORD g_dwThreadIdAttachTo = 0;  // 0=System-wide; Non-zero=specifc thread

HWND g_hwndMain = NULL;

DWORD g_msec_mouse_down = 0;
const int MSEC_CAPTURE_HOLD = 200; 

///////////////////////////////////////////////////////////////////////////////

BOOL u_AttachThreadInput(DWORD tid_from, DWORD tid_to, BOOL fAttach)
{
	if (tid_from == tid_to)
		return FALSE;

	const TCHAR *torf = fAttach ? _T("TRUE") : _T("FALSE");

	dbgprint(_T("LISWatch: AttachThreadInput(%d, %d, %s)..."), tid_from, tid_to, torf);

	BOOL succ = AttachThreadInput(tid_from, tid_to, fAttach);
	if (!succ)
	{
		DWORD winerr = GetLastError();
		vaMsgBoxWinErr(g_hwndMain, _T("AttachThreadInput(%d, %d, %s) fail."),
			tid_from, tid_to, torf);

		dbgprint(_T("LISWatch: AttachThreadInput(%d, %d, %s) fail with winerr=%d."),
			tid_from, tid_to, torf, winerr);
	}
	return succ;
}



BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	g_hwndMain = hwnd;
	chSETDLGICONS(hwnd, IDI_LISWATCH);

	TCHAR title[50] = {};
	int bufsize = sizeof(title) / sizeof(title[0]) - 1;
	DWORD pid = GetCurrentProcessId();
	DWORD tid = GetCurrentThreadId();
	_sntprintf_s(title, bufsize, _T("LISWatch (pid=%d, tid=%d)"), pid, tid);
	SetWindowText(hwnd, title);

	// Update our contents periodically
	g_uTimerId = SetTimer(hwnd, g_uTimerId, TIMER_DELAY, NULL);

	// Make our window on top of all others
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


//void Dlg_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y,	UINT keyFlags) 
void ShowHelp()
{
	chMB("To monitor a specific thread, press and hold the left mouse button in "
		"the main window, then drag and release it in the desired window.\n"
		"\n"
		"To monitor all threads, drag and release left mouse button in "
		"LISWatch window itself.");
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, 	UINT keyFlags) 
{
	// Set capture to ourself and change the mouse cursor
	SetCapture(hwnd);
	SetCursor(LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_EYES)));

	g_msec_mouse_down = GetTickCount();
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags) 
{
	if (GetCapture() == hwnd) {

		ReleaseCapture();

		if (GetTickCount() - g_msec_mouse_down < MSEC_CAPTURE_HOLD) {
			// Chj: Not enough dragging time, consider it a mouse click, so don't do re-attach.
			return;
		}

		// If we had mouse capture set, get the ID of the thread that
		// created the window that is under the mouse cursor.
		POINT pt;
		pt.x = LOWORD(GetMessagePos());
		pt.y = HIWORD(GetMessagePos());

		// If we're attached to a thread, first detach from it
		if (g_dwThreadIdAttachTo != 0) {
			BOOL succ = u_AttachThreadInput(GetCurrentThreadId(), g_dwThreadIdAttachTo, FALSE);
			if (!succ) {
				// Chj: The target thread probably has exited, so abandon it.
				g_dwThreadIdAttachTo = 0;
			}
		}

		g_dwThreadIdAttachTo = GetWindowThreadProcessId(
			ChildWindowFromPointEx(GetDesktopWindow(), pt, CWP_SKIPINVISIBLE),
			NULL);

		if (g_dwThreadIdAttachTo == GetCurrentThreadId()) {

			// The mouse button is released on one of our windows; 
			// monitor local-input state on a system-wide basis
			g_dwThreadIdAttachTo = 0;

		}
		else {

			// The mouse button is released on a window that our thread didn't
			// create; monitor local input state for that thread only.
			u_AttachThreadInput(GetCurrentThreadId(), g_dwThreadIdAttachTo, TRUE);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////

void u_GetWindowText(HWND hwnd, TCHAR textbuf[], int bufchars)
{
	// Chj: MSDN requires that cross-process get-window-text can only be achieved
	// by explicitly sending WM_GETTEXT messeage, not by calling GetWindowText().
	SendMessage(hwnd, WM_GETTEXT, bufchars, (LPARAM)textbuf);
}


static void CalcWndText(HWND hwnd, PTSTR szBuf, int nLen) 
{

	if (hwnd == (HWND)NULL) {
		lstrcpy(szBuf, TEXT("(no window)"));
		return;
	}

	if (!IsWindow(hwnd)) {
		lstrcpy(szBuf, TEXT("(invalid window)"));
		return;
	}

	TCHAR szClass[50], szCaption[50], szBufT[150];
	GetClassName(hwnd, szClass, chDIMOF(szClass));
	
	u_GetWindowText(hwnd, szCaption, chDIMOF(szCaption));
	
	wsprintf(szBufT, TEXT("[%s] %s"), (PTSTR)szClass,
		(szCaption[0] == 0) ? (PTSTR)TEXT("(no caption)") : (PTSTR)szCaption);
	_tcsncpy(szBuf, szBufT, nLen - 1);
	szBuf[nLen - 1] = 0; // Force zero-terminated string
}


//////////////////////////////////////////////////////////////


void Dlg_OnTimer(HWND hwnd, UINT id) 
{
	static bool s_isInTimer = false;

	if (s_isInTimer) {
		// avoid timer re-trigger when the timer WndProc display a message-box
		return;
	}

	s_isInTimer = true;

	TCHAR szBuf[100] = TEXT("System-wide");
	int nBuf = chDIMOF(szBuf);
	HWND hwndForeground = GetForegroundWindow();
	DWORD dwThreadIdAttachTo = g_dwThreadIdAttachTo;

	if (dwThreadIdAttachTo == 0) {

		if (hwndForeground)
		{
			// If monitoring local input state system-wide, attach our input
			// state to the thread that created the current foreground window.
			dwThreadIdAttachTo = GetWindowThreadProcessId(hwndForeground, NULL);

			BOOL succ = u_AttachThreadInput(GetCurrentThreadId(), dwThreadIdAttachTo, TRUE);
			if( succ)
				_sntprintf_s(szBuf, nBuf, _T("System-wide (now attach to %d)"), dwThreadIdAttachTo);
		}
		else
		{	// Chj: We may see this when we user clicks on a frozen window.
			// That frozen may due to it is being attached to and paused by a debugger.
			_sntprintf_s(szBuf, nBuf, _T("%s"), _T("hwndForground=NULL"));
		}
	}
	else {

		wsprintf(szBuf, TEXT("%d"), dwThreadIdAttachTo);
	}

	SetWindowText(GetDlgItem(hwnd, IDC_THREADID), szBuf);

	CalcWndText(GetFocus(), szBuf, chDIMOF(szBuf));
	SetWindowText(GetDlgItem(hwnd, IDC_WNDFOCUS), szBuf);

	CalcWndText(GetActiveWindow(), szBuf, chDIMOF(szBuf));
	SetWindowText(GetDlgItem(hwnd, IDC_WNDACTIVE), szBuf);

	CalcWndText(GetCapture(), szBuf, chDIMOF(szBuf));
	SetWindowText(GetDlgItem(hwnd, IDC_WNDCAPTURE), szBuf);

	CalcWndText(hwndForeground, szBuf, chDIMOF(szBuf));
	SetWindowText(GetDlgItem(hwnd, IDC_WNDFOREGRND), szBuf);

	if (g_dwThreadIdAttachTo == 0) {
		// If monitoring local input state system-wide, detach our input
		// state from the thread that created the current foreground window.
		if (dwThreadIdAttachTo != 0) {
			u_AttachThreadInput(GetCurrentThreadId(), dwThreadIdAttachTo, FALSE);
		}
	}

	s_isInTimer = false;
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{

	switch (id) {
	case IDC_HELP:
		ShowHelp();
		break;
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{

	switch (uMsg) {
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, Dlg_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_TIMER, Dlg_OnTimer);
//		chHANDLE_DLGMSG(hwnd, WM_RBUTTONDOWN, Dlg_OnRButtonDown);
		chHANDLE_DLGMSG(hwnd, WM_LBUTTONDOWN, Dlg_OnLButtonDown);
		chHANDLE_DLGMSG(hwnd, WM_LBUTTONUP, Dlg_OnLButtonUp);
	}
	return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int)
{

	DialogBox(hinstExe, MAKEINTRESOURCE(IDD_LISWATCH), NULL, Dlg_Proc);
	return(0);
}


//////////////////////////////// End of File //////////////////////////////////

