#pragma once
#include <windows.h>
#include <d3dtypes.h>

#include "CxxDialog.h"

class CMyD3DApplication;

class ParamDialog : public CxxDialog
{
public:
	ParamDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void GuiToData(HWND hdlg);

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnClose(HWND hwnd);

private:
	BOOL m_isLightAnimation;
	BOOL m_isCameraAnimation;

	D3DLIGHTTYPE m_lighttype;

	friend class CMyD3DApplication;
};

