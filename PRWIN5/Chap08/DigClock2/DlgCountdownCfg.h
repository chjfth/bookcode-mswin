#pragma once

#include "CxxDialogBase.h"
#include "utils.h"

// This implements the floating countdown-cfg panel (floatbar).


class CDlgCountdownCfg : public CxxDialogBase
{
public:
	CDlgCountdownCfg() {}

	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) cxx11_override;


protected:
	// Note: This OnXXX functions needs to have `HWND dlg` as first param, because 
	// WindowsX.h's HANDLE_WM_xxx(HANDLE_WM_INITDIALOG, HANDLE_WM_COMMAND etc) requires it. 
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);

private:
	// ...	
};

