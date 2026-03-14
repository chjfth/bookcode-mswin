#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <mswin/utils_wingui.h>
#include <mswin/Editbox_EnableKbdAdjustFloatnum.h>

#include "resource.h"
#include "CxxParamDialog.h"

//
// Chj constants of default values
//
#define C_PointLightRadius 4.5f
#define C_PointLightHeight 2.0f

#define C_CameraDistance 4.0f
#define C_CameraOrbitDegree -135.0f
#define C_CameraHeight 3.0f


float getDlgItemFloat(HWND hdlg, int Uic)
{
	TCHAR sztext[100] = {};
	GetDlgItemText(hdlg, Uic, sztext, ARRAYSIZE(sztext));
	return (float)_ttof(sztext);
}

ParamDialog::ParamDialog()
{
	m_lighttype = D3DLIGHT_POINT;
	m_isPointLightLatitude = false;
	m_PointLightHeight = C_PointLightHeight;

	m_CameraWaggleDegree = 0;
}

//enum NumWrap_et { NumWrap_no=0, NumWrap_yes=1 };
#define NumWrap_no FALSE
#define NumWrap_yes TRUE


void ParamDialog::InitParams()
{
	HWND hdlg = m_hwndDlg;
	HWND hedit = NULL, hckbox = NULL;
	float def_val=0, min_val=0, max_val=0;
	EditboxKAF_err kerr;

	// Light-type radio group
	mc_LightType.Init(hdlg, IDC_RDO_PointLight, IDC_RDO_DirectionalLight);
	m_saLiveUic.AppendTail(&mc_LightType);

	// Light Animation checkbox
	hckbox = GetDlgItem(hdlg, IDC_CKB_LightAnimation);
	mc_LightAnimation.Init(hckbox, BST_CHECKED);
	m_saLiveUic.AppendTail(&mc_LightAnimation);

	// Camera Animation checkbox
	hckbox = GetDlgItem(hdlg, IDC_CKB_CameraAnimation);
	mc_CameraAnimation.Init(hckbox, BST_CHECKED);
	m_saLiveUic.AppendTail(&mc_CameraAnimation);

	// PointLight Radius
	def_val = C_PointLightRadius;
	min_val = 0.1f;
	max_val = 10.0f;
	hedit = GetDlgItem(hdlg, IDE_PointLightRadius);
	mc_PointLightRadius.Init(hedit, def_val, min_val, max_val);
	m_saLiveUic.AppendTail(&mc_PointLightRadius);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val, 
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Point-light radius around Y-axis.")
		);
	assert(!kerr);

	// Camera Height editbox
	def_val = C_CameraHeight; 
	min_val = -15.0f; 
	max_val = 15.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraHeight);
	mc_CameraHeight.Init(hedit, def_val, min_val, max_val);
	m_saLiveUic.AppendTail(&mc_CameraHeight);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Camera height from the ground(XZ-plane).")
		);
	assert(!kerr);

	// Camera Distance editbox
	def_val = C_CameraDistance;
	min_val = 0.0f;
	max_val = 10.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraDistance);
	mc_CameraDistance.Init(hedit, def_val, min_val, max_val);
	m_saLiveUic.AppendTail(&mc_CameraDistance);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Camera distance from the Y-axis.")
		);
	assert(!kerr);

	// Camera Orbit Degree editbox
	def_val = C_CameraOrbitDegree;
	min_val = -180.0f;
	max_val = +180.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraOrbitDegree);
	mc_CameraOrbitDegree.Init(hedit, def_val, min_val, max_val);
	m_saLiveUic.AppendTail(&mc_CameraOrbitDegree);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		1.0f, _T("%.1f"), NumWrap_yes, // step_val, fmt
		_T("Camera orbit degree on the latitude. 0~90 means from +X to +Z.")
		);
}

void ParamDialog::DataFromGui()
{
	HWND hdlg = m_hwndDlg;

	for(int i=0; i<m_saLiveUic.CurrentEles(); i++)
	{
		m_saLiveUic[i]->DataFromUic();
	}

	m_lighttype = D3DLIGHT_POINT;
	int uicLightType = mc_LightType.GetActive();
	if(uicLightType==IDC_RDO_SpotLight)
		m_lighttype = D3DLIGHT_SPOT;
	else if(uicLightType==IDC_RDO_DirectionalLight)
		m_lighttype = D3DLIGHT_DIRECTIONAL;
	//
	if(uicLightType==IDC_RDO_PointLight)
		m_isPointLightLatitude = false;

	else if(uicLightType==IDC_RDO_PointLight2)
		m_isPointLightLatitude = true;

	enableDlgItem(hdlg, IDE_PointLightRadius, m_isPointLightLatitude);
			// 	m_PointLightRadius = getDlgItemFloat(hdlg, IDE_PointLightRadius);
			// 	//
			// 	HWND heditPointLightRadius = GetDlgItem(hdlg, IDE_PointLightRadius);
			// 	if(IsDlgButtonChecked(hdlg, IDC_RDO_PointLight2))
			// 		EnableWindow(heditPointLightRadius, TRUE);
			// 	else
			// 		EnableWindow(heditPointLightRadius, FALSE);

	m_CameraWaggleDegree = 0.0f;

}

void ParamDialog::SetGui_CameraOrbitDegreeLive(float degree)
{
	vaSetDlgItemText(m_hwndDlg, IDE_CameraOrbitDegreeLive, _T("%.1f"), degree);
}

void ParamDialog::OnCommand(HWND hdlg, int uic, HWND hwndCtl, UINT codeNotify) 
{
//	DataFromGui();

	switch (uic) 
	{{
	case IDC_BTN_ResetParams:
	{
		for(int i=0; i<m_saLiveUic.CurrentEles(); i++)
		{
			m_saLiveUic[i]->Reset();
		}
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

	if(codeNotify==EN_CHANGE || codeNotify==BN_CLICKED)
	{
		if(uic!=IDE_CameraOrbitDegreeLive && uic!=IDC_BTN_ResetParams)
			DataFromGui();
	}
}

void ParamDialog::OnClose(HWND hdlg)
{
	// Do nothing, don't close the modeless dialog.
}


BOOL ParamDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	InitParams();
	DataFromGui();

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
