#include <mswin/DlgTooltipEasy.h>
#include <mswin/Editbox_EnableKbdAdjustIntnum.h>

#include "resource.h"

#include "DlgCountdownCfg.h"

#include "utils_wingui.h"

static const TCHAR* DlgtteGetText_CountdownEditbox(HWND hwndCtl, void *userctx)
{
	(void)userctx;
	static DWORD s_dwInitial = GetTickCount();

	DWORD nowtick = GetTickCount();
	if (nowtick - s_dwInitial < 5000)
	{
		// only display the tooltip for first 5 seconds
		return _T("Use keyboard Up/Down arrow to adjust time values.");
	}
	else
		return NULL;
}


BOOL CDlgCountdownCfg::OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	Sdring cfghms = CAppWin_GetCountDownCfg();
	SetDlgItemText(hDlg, IDC_EDIT_COUNTDOWN, cfghms);

	HWND hEdit = GetDlgItem(hDlg, IDC_EDIT_COUNTDOWN);
	Editbox_EnableKbdAdjustIntnum(hEdit, 0, 59, 1, true, 2);

	// Place editbox caret at end, bcz when debugging, we fiddle with seconds often.
	int textlen = GetWindowTextLength(hEdit);
	Edit_SetSel(hEdit, textlen, textlen);

	Dlgtte_EnableTooltip(hEdit, DlgtteGetText_CountdownEditbox, nullptr);

	// return FALSE to tell dialog manager: Do NOT set keyboard focus to the editbox,
	// bcz, I want the default focus on main-window itself, so that user can move the
	// main-window by keyboard arrow keys.
	return FALSE;
}

void CDlgCountdownCfg::OnCommand(HWND hDlg, int idcmd, HWND hwndCtl, UINT codeNotify)
{
	if (idcmd == IDB_StartCountDown)
	{
		TCHAR szHMS[20] = {};
		GetDlgItemText(hDlg, IDC_EDIT_COUNTDOWN, szHMS, ARRAYSIZE(szHMS)-1);
		
		bool succ = CAppWin_StartCountDown(szHMS);
		if (!succ)
			return;

		SetDlgItemText(hDlg, IDB_StartCountDown, _T("Restart countdown"));
	}
}

/*
INT_PTR CALLBACK
Dlgproc_CountdownCfg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_dlgMSG(hwnd, WM_INITDIALOG, OnInitDialog);
		HANDLE_dlgMSG(hwnd, WM_COMMAND, OnCommand);
	}
	return FALSE; // Let Dlg-manager do default for current message.
}
*/


INT_PTR CDlgCountdownCfg::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hdlg = m_hwndDlg;
	switch (uMsg)
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG, OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND, OnCommand);
	}
	return FALSE; // Let Dlg-manager do default for current message.
}
