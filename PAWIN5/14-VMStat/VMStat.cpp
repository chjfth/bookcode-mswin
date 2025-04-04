/******************************************************************************
Module:  VMStat.cpp
Notices: Copyright (c) 2008 Jeffrey Richter & Christophe Nasarre

[2024-12-18] Chj:
* Use GlobalMemoryStatusEx() instead of GlobalMemoryStatus(), so that :
  this 32bit exe, when run on 64bit Win7, reports realistic values.
* Use BigNum64ToString() to add thousand separator for easier reading.
* Show EXE is 32bit or 64bit on window title.

******************************************************************************/


#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "Resource.h"
#include <psapi.h>
#include <StrSafe.h>
#include "vaDbg.h"

// Look for the .lib corresponding to psapi.dll
#pragma comment (lib,"psapi.lib")


///////////////////////////////////////////////////////////////////////////////


// The update timer's ID
#define IDT_UPDATE   1


///////////////////////////////////////////////////////////////////////////////


BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_VMSTAT);

	// Chj: Show EXE is 32bit or 64bit on window title.
	int exebits = sizeof(void*)==4 ? 32 : 64;
	vaSetWindowText(hwnd, _T("VMStat (%dbit exe)"), exebits);

	// Set a timer so that the information updates periodically.
	SetTimer(hwnd, IDT_UPDATE, 1 * 1000, NULL);

	// Force a timer message for the initial update.
	FORWARD_WM_TIMER(hwnd, IDT_UPDATE, SendMessage);
	return(TRUE);
}

void Dlg_OnTimer(HWND hwnd, UINT id) 
{
	// Initialize the structure length before calling GlobalMemoryStatus.
	MEMORYSTATUSEX ms = { sizeof(ms) };
	GlobalMemoryStatusEx(&ms);

	TCHAR sztmp[40] = {};
	TCHAR szData[512] = { 0 };
	_sntprintf_s(szData, _TRUNCATE, _T("%d%%\n"), ms.dwMemoryLoad);

	_tcscat_s(szData, BigNum64ToString(ms.ullTotalPhys, sztmp, _countof(sztmp)));
	_tcscat_s(szData, _T("\n"));
	_tcscat_s(szData, BigNum64ToString(ms.ullAvailPhys, sztmp, _countof(sztmp)));
	_tcscat_s(szData, _T("\n"));
	_tcscat_s(szData, BigNum64ToString(ms.ullTotalPageFile, sztmp, _countof(sztmp)));
	_tcscat_s(szData, _T("\n"));
	_tcscat_s(szData, BigNum64ToString(ms.ullAvailPageFile, sztmp, _countof(sztmp)));
	_tcscat_s(szData, _T("\n"));
	_tcscat_s(szData, BigNum64ToString(ms.ullTotalVirtual, sztmp, _countof(sztmp)));
	_tcscat_s(szData, _T("\n"));
	_tcscat_s(szData, BigNum64ToString(ms.ullAvailVirtual, sztmp, _countof(sztmp)));

/* old code:
	StringCchPrintf(szData, _countof(szData), 
		TEXT("%d\n%d\n%I64d\n%I64d\n%I64d\n%I64d\n%I64d"),
		ms.dwMemoryLoad, ms.dwTotalPhys, 
		(__int64) ms.dwAvailPhys,     (__int64) ms.dwTotalPageFile, 
		(__int64) ms.dwAvailPageFile, (__int64) ms.dwTotalVirtual, 
		(__int64) ms.dwAvailVirtual);
*/

	SetDlgItemText(hwnd, IDC_DATA, szData);

	// Get the current process working set and private bytes.
	PROCESS_MEMORY_COUNTERS_EX pmc = { sizeof(PROCESS_MEMORY_COUNTERS_EX) };
	GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&pmc, sizeof(pmc));
	StringCchPrintf(szData, _countof(szData), 
		TEXT("%I64d K\n%I64d K"),
		(__int64) pmc.WorkingSetSize / 1024,
		(__int64) pmc.PrivateUsage / 1024);
	SetDlgItemText(hwnd, IDC_PROCESSDATA, szData);
}


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) {
	case IDCANCEL:
		KillTimer(hwnd, IDT_UPDATE);
		EndDialog(hwnd, id);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////

INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	switch (uMsg) {
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND,    Dlg_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_TIMER,      Dlg_OnTimer);
	}
	return(FALSE);
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) {

	DialogBox(hinstExe, MAKEINTRESOURCE(IDD_VMSTAT), NULL, Dlg_Proc);
	return(0);
}

//////////////////////////////// End of File //////////////////////////////////
