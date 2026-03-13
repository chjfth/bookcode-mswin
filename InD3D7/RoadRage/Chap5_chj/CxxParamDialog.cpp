#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <mswin/utils_wingui.h>
#include <mswin/Editbox_EnableKbdAdjustFloatnum.h>

#include "resource.h"
#include "CxxParamDialog.h"

float getDlgItemFloat(HWND hdlg, int Uic)
{
	TCHAR sztext[100] = {};
	GetDlgItemText(hdlg, Uic, sztext, ARRAYSIZE(sztext));
	return (float)_ttof(sztext);
}

ParamDialog::ParamDialog()
{
	m_isLightAnimation = FALSE;
	m_isCameraAnimation = FALSE;

	m_CameraHeight = 0;
	m_CameraDistance = 0;
	m_CameraOrbitDegree = 0;
//	m_CameraOrbitDegreeLive = 0;
	m_CameraWaggleDegree = 0;
}

enum NumWrap_et { NumWrap_no=0, NumWrap_yes=1 };

void ParamDlg_SetEditboxParams(HWND hdlg, int idcEditbox,
	float init_val,
	float min_val, float max_val, float step_val, const TCHAR *szfmt,
	NumWrap_et numwrap,
	const TCHAR *helptext)
{
	HWND hedit = GetDlgItem(hdlg, idcEditbox);
	vaSetWindowText(hedit, szfmt, init_val);
	EditboxKAF_err kerr = Editbox_EnableKbdAdjustFloatnum(hedit, 
		min_val, max_val, step_val, szfmt, 
		numwrap==NumWrap_yes ? true : false,
		helptext);
	assert(!kerr);
}

void ParamDialog::ResetParams()
{
	HWND hdlg = m_hwndDlg;

	CheckDlgButton(hdlg, IDC_CKB_LightAnimation, TRUE);
	CheckDlgButton(hdlg, IDC_CKB_CameraAnimation, TRUE);
	CheckDlgButton(hdlg, IDC_RDO_PointLight, TRUE);

	ParamDlg_SetEditboxParams(hdlg, IDE_CameraHeight, 
		C_CameraHeight,
		-10.0f, 10.0f, 0.1f, _T("%.1f"),
		NumWrap_no,
		_T("Camera height from the ground(XZ-plane).")
		);

	ParamDlg_SetEditboxParams(hdlg, IDE_CameraDistance,
		C_CameraDistance,
		-10.0f, 10.0f, 0.1f, _T("%.1f"),
		NumWrap_no,
		_T("Camera distance from the Y axis.")
		);

	ParamDlg_SetEditboxParams(hdlg, IDE_CameraOrbitDegree,
		C_CameraOrbitDegree,
		-180.0f, 180.0f, 1.0f, _T("%.1f"),
		NumWrap_yes,
		_T("Camera orbit degree on the latitude. 0~90 means from +X to +Z.")
		);

	GuiToData();

}

void ParamDialog::GuiToData()
{
	HWND hdlg = m_hwndDlg;

	m_isLightAnimation = IsDlgButtonChecked(hdlg, IDC_CKB_LightAnimation);
	m_isCameraAnimation = IsDlgButtonChecked(hdlg, IDC_CKB_CameraAnimation);

	m_lighttype = D3DLIGHT_POINT;
	if(IsDlgButtonChecked(hdlg, IDC_RDO_SpotLight))
		m_lighttype = D3DLIGHT_SPOT;
	else if(IsDlgButtonChecked(hdlg, IDC_RDO_DirectionalLight))
		m_lighttype = D3DLIGHT_DIRECTIONAL;

	m_CameraHeight = getDlgItemFloat(hdlg, IDE_CameraHeight);
	m_CameraDistance = getDlgItemFloat(hdlg, IDE_CameraDistance);
	m_CameraOrbitDegree = getDlgItemFloat(hdlg, IDE_CameraOrbitDegree);
	
	m_CameraWaggleDegree = 0.0f;
}

void ParamDialog::SetGui_CameraOrbitDegreeLive(float degree)
{
	vaSetDlgItemText(m_hwndDlg, IDE_CameraOrbitDegreeLive, _T("%.1f"), degree);
}

void ParamDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	GuiToData();

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

	if(codeNotify==EN_CHANGE)
	{
		GuiToData();
	}
}

void ParamDialog::OnClose(HWND hdlg)
{
	// Do nothing, don't close the modeless dialog.
}


BOOL ParamDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	ResetParams();
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
