#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

// A different type of dialog procedure
// https://devblogs.microsoft.com/oldnewthing/20031112-00/?p=41863
/*
Chj: Purpose of this program:

In normal paradigm like that told in PRWIN5, Windows dialog-box manager always do its
"default processing" AFTER our dialog-box procedure. But what if we want to do some
post-default-action after that "default processing"?

Yes, we can do it.
*/

class WLDialogBox // WndProc-Like DialogBox
{
public:
	virtual LRESULT WLDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return DefDlgProcEx(hdlg, uMsg, wParam, lParam, &m_fRecursing);
		// -- Important: DefDlgProcEx() macro sets m_fRecursing=TRUE, and this TRUE
		//    is later checked by CheckDefDlgRecursion().
	}

	INT_PTR DoModal(HINSTANCE hinst, LPCTSTR pszTemplate,
		HWND hwndParent)
	{
		m_fRecursing = FALSE;
		return DialogBoxParam(hinst, pszTemplate, hwndParent, s_DlgProc, (LPARAM)this);
	}

private:
	static INT_PTR CALLBACK s_DlgProc(
		HWND hdlg, UINT uMsg,	WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_INITDIALOG) {
			SetWindowLongPtr(hdlg, DWLP_USER, lParam);
			// Do not return yet.
		}

		WLDialogBox *self = (WLDialogBox*)GetWindowLongPtr(hdlg, DWLP_USER);
		if (!self) {
			return FALSE;
		}

		CheckDefDlgRecursion(&self->m_fRecursing); 
		// -- note: This macro contains conditional `return`.
		
		LRESULT lre = self->WLDlgProc(hdlg, uMsg, wParam, lParam);
		return SetDlgMsgResult(hdlg, uMsg, lre);
	}

private:
	BOOL m_fRecursing;
};

class SampleWLDlg : public WLDialogBox
{
	virtual LRESULT WLDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) // override
	{
		switch (uMsg) {
			HANDLE_MSG(hdlg, WM_COMMAND, OnCommand);
			HANDLE_MSG(hdlg, WM_SETCURSOR, OnSetCursor);
		}

		LRESULT lre = __super::WLDlgProc(hdlg, uMsg, wParam, lParam);

		// [2025-05-24] Chj: We can do some post-default-action here.

		return lre;
	};

	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify)
	{
		switch (id) {
		case IDCANCEL:
			MessageBox(hdlg, TEXT("Bye"), TEXT("Title"), MB_OK);
			EndDialog(hdlg, 1);
			break;
		}
	}

	BOOL OnSetCursor(HWND hdlg, HWND hwndCursor, UINT codeHitTest, UINT mousemsg)
	{
		if (codeHitTest == HTCAPTION) {
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			return TRUE;
		}

		return FORWARD_WM_SETCURSOR(hdlg, hwndCursor,
			codeHitTest, mousemsg, __super::WLDlgProc);
	}
};


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nShowCmd)
{
	SampleWLDlg dlg;
	dlg.DoModal(hinst, MAKEINTRESOURCE(1), NULL);
	return 0;
}
