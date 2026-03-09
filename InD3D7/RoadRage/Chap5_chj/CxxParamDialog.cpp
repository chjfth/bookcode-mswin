#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <mswin/utils_wingui.h>
#include <mswin/Editbox_EnableKbdAdjustFloatnum.h>

#include "resource.h"
#include "CxxParamDialog.h"


ParamDialog::ParamDialog()
{
	m_isLightAnimation = FALSE;
	m_isCameraAnimation = FALSE;
}

void ParamDialog::ResetParams()
{
	m_CameraDistance = C_CameraDistance;
	m_CameraSlideDegree = C_CameraSlideDegree;
	m_CameraHeight = C_CameraHeight;
	m_CameraWaggleDegree = 0.0f;
}

void ParamDialog::GuiToData(HWND hdlg)
{
	m_isLightAnimation = IsDlgButtonChecked(hdlg, IDC_CKB_LightAnimation);
	m_isCameraAnimation = IsDlgButtonChecked(hdlg, IDC_CKB_CameraAnimation);

	m_lighttype = D3DLIGHT_POINT;
	if(IsDlgButtonChecked(hdlg, IDC_RDO_SpotLight))
		m_lighttype = D3DLIGHT_SPOT;
	else if(IsDlgButtonChecked(hdlg, IDC_RDO_DirectionalLight))
		m_lighttype = D3DLIGHT_DIRECTIONAL;
}

void ParamDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	GuiToData(hdlg);

	switch (id) 
	{{
	case IDC_BTN_UPDATE:
	{
		break;
	}
	case IDOK:
	case IDCANCEL:
	{
		// Don't close dialog
		// EndDialog(hdlg, id); 
		break;
	}
	}}
}

void ParamDialog::OnClose(HWND hdlg)
{
	// Do nothing, don't close the modeless dialog.
}


BOOL ParamDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	CheckDlgButton(hdlg, IDC_CKB_LightAnimation, TRUE);
	CheckDlgButton(hdlg, IDC_CKB_CameraAnimation, TRUE);
	CheckDlgButton(hdlg, IDC_RDO_PointLight, TRUE);

	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_CameraDistance);
	SetWindowText(hedit, _T("1.0234"));
	EditboxKAF_err kerr = Editbox_EnableKbdAdjustFloatnum(hedit, -2.0, 2.0, 0.1, _T("%g"), false);
	assert(!kerr);

	GuiToData(hdlg);

	return TRUE;
}

INT_PTR ParamDialog::DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
		HANDLE_dlgMSG(hdlg, WM_CLOSE,         OnClose);
	}
	return FALSE;
}
