#include <assert.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <mswin/utils_wingui.h>
#include <mswin/Editbox_EnableKbdAdjustFloatnum.h>

#include "resource.h"
#include "CxxParamDialog.h"
#include "verstr.h"

//
// Chj constants of default values
//
#define C_PointLightRadius 4.5f
#define C_PointLightHeight 2.0f

#define C_CameraDistance 4.0f
#define C_CameraOrbitDegree -135.0f
#define C_CameraHeight 3.0f
#define C_CameraWaggleDegreeMax 36.0f

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
}

//enum NumWrap_et { NumWrap_no=0, NumWrap_yes=1 };
#define NumWrap_no FALSE
#define NumWrap_yes TRUE


void ParamDialog::InitParams()
{
	static const TCHAR *ini_filename = _T("In3D-Chap5.ini");
	static const TCHAR *ini_secname = _T("cfg");
	m_xini.LoadIni(&ini_filename, 1);

	HWND hdlg = m_hwndDlg;
	HWND hedit = NULL, hckbox = NULL;
	float def_val=0, min_val=0, max_val=0;
	EditboxKAF_err kerr;

	// Light-type radio group
	mc_LightType.Init(hdlg, IDC_RDO_PointLight, IDC_RDO_DirectionalLight);
	m_xini.AddItem(ini_secname, _T("rdb_LightType"), &mc_LightType);

	// Light Animation checkbox
	hckbox = GetDlgItem(hdlg, IDC_CKB_LightAnimation);
	mc_LightAnimation.Init(hckbox, BST_CHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_LightAnimation"), &mc_LightAnimation);

	// Camera Animation checkbox
	hckbox = GetDlgItem(hdlg, IDC_CKB_CameraAnimation);
	mc_CameraAnimation.Init(hckbox, BST_CHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_CameraAnimation"), &mc_CameraAnimation);

	// PointLight Radius
	def_val = C_PointLightRadius;
	min_val = 0.1f;
	max_val = 10.0f;
	hedit = GetDlgItem(hdlg, IDE_PointLightRadius);
	mc_PointLightRadius.Init(hedit, def_val, min_val, max_val);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val, 
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Point-light radius around Y-axis.")
		);
	m_xini.AddItem(ini_secname, _T("edt_PointLightRadius"), &mc_PointLightRadius);
	assert(!kerr);

	// show LightXYZ editbox (readonly)
	hedit = GetDlgItem(hdlg, IDC_EDO_LightXYZ);
	mc_LightXYZ.Init(hedit, NULL);

	// Camera Height editbox
	def_val = C_CameraHeight; 
	min_val = -15.0f; 
	max_val = 15.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraHeight);
	mc_CameraHeight.Init(hedit, def_val, min_val, max_val);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Camera height from the ground(XZ-plane).")
		);
	assert(!kerr);
	m_xini.AddItem(ini_secname, _T("edt_CameraHeight"), &mc_CameraHeight);

	// Camera Distance editbox
	def_val = C_CameraDistance;
	min_val = 0.0f;
	max_val = 10.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraDistance);
	mc_CameraDistance.Init(hedit, def_val, min_val, max_val);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		0.1f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Camera distance from the Y-axis.")
		);
	assert(!kerr);
	m_xini.AddItem(ini_secname, _T("edt_CameraDistance"), &mc_CameraDistance);

	// Camera Orbit Degree editbox
	def_val = C_CameraOrbitDegree;
	min_val = -180.0f;
	max_val = +180.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraOrbitDegree);
	mc_CameraOrbitDegree.Init(hedit, def_val, min_val, max_val);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		1.0f, _T("%.1f"), NumWrap_yes, // step_val, fmt
		_T("Camera orbit degree on the latitude. 0~90 means from +X to +Z.")
		);
	assert(!kerr);
	m_xini.AddItem(ini_secname, _T("edt_CameraOrbitDegree"), &mc_CameraOrbitDegree);

	// Camera Waggle Degree Max editbox
	def_val = C_CameraWaggleDegreeMax;
	min_val = 0.0f;
	max_val = 90.0f;
	hedit = GetDlgItem(hdlg, IDE_CameraWaggleDegreeMax);
	mc_CameraWaggleDegreeMax.Init(hedit, def_val, min_val, max_val);
	kerr = Editbox_EnableKbdAdjustFloatnum(hedit, min_val, max_val,
		1.0f, _T("%.1f"), NumWrap_no, // step_val, fmt
		_T("Camera waggle back-and-forth max degree, around the orbit-degree value.")
		);
	assert(!kerr);
	m_xini.AddItem(ini_secname, _T("edt_CameraWaggleDegreeMax"), &mc_CameraWaggleDegreeMax);

	// Render wall/ball as grid checkbox
	hckbox = GetDlgItem(hdlg, IDC_CKB_BackWallGrid);
	mc_IsBackWallGrid.Init(hckbox, BST_UNCHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_BackWallGrid"), &mc_IsBackWallGrid);
	//
	hckbox = GetDlgItem(hdlg, IDC_CKB_BottomGrid);
	mc_IsBottomGrid.Init(hckbox, BST_UNCHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_BottomGrid"), &mc_IsBottomGrid);
	//
	hckbox = GetDlgItem(hdlg, IDC_CKB_RightWallGrid);
	mc_IsRightWallGrid.Init(hckbox, BST_UNCHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_RightWallGrid"), &mc_IsRightWallGrid);
	//
	hckbox = GetDlgItem(hdlg, IDC_CKB_BallGrid);
	mc_IsBallGrid.Init(hckbox, BST_UNCHECKED);
	m_xini.AddItem(ini_secname, _T("ckb_BallGrid"), &mc_IsBallGrid);

	// Zbuffer radio-group
	mc_Zbuffer.Init(hdlg, IDC_RDO_ZbufferNo, IDC_RDO_ZbufferW);
	m_xini.AddItem(ini_secname, _T("rdb_Zbuffer"), &mc_Zbuffer);

	// Cull-mode radio-group
	mc_Cullmode.Init(hdlg, IDC_RDO_CullNone, IDC_RDO_CullCounterClockwise, IDC_RDO_CullCounterClockwise);
	m_xini.AddItem(ini_secname, _T("rdb_Cullmode"), &mc_Cullmode);

	// Edit Comment
	HWND hedtComment = GetDlgItem(hdlg, IDC_EDIT_COMMENT);
	mc_edtComment.Init(hedtComment, _T("Free comments here."));
	m_xini.AddItem(ini_secname, _T("edt_Comment"), &mc_edtComment);;
}

void ParamDialog::DataFromGui()
{
	HWND hdlg = m_hwndDlg;

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
}

void ParamDialog::SetGui_CameraOrbitDegreeLive(float degree)
{
	vaSetDlgItemText(m_hwndDlg, IDE_CameraOrbitDegreeLive, _T("%.1f"), degree);
}

void ParamDialog::OnCommand(HWND hdlg, int uic, HWND hwndCtl, UINT codeNotify) 
{
	switch (uic) 
	{{
	case IDC_BTN_ResetParams:
	{
		DlgboxPeeker *peeker = GetDlgboxPeeker(hdlg);
		peeker->ResetAllUicContent();
		m_xini.ResetDefault();
		break;
	}
	case IDC_BTN_HELP:
	{
		vaMsgBox(hdlg, 0, _T("info"), _T("Nothing yet."));
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
	vaSetWindowText(hdlg, _T("InD3D Chap5_chj v%s Parameters"), _T(VER_STR));

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

	if(uMsg==liveuic::wmDataChanged)
	{
		DataFromGui();
	}
	else if (uMsg==liveuic::wmUicFocusLost)
	{
		m_xini.SaveIni();
	}

	return FALSE;
}
