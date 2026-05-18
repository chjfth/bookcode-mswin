#pragma once
#include <windows.h>
#include <d3dtypes.h>

#include <TScalableArray.h>

#include <mswin/LiveUicXString.h>
	using namespace liveuic;

#include <DataXIni.h>

#include "CxxDialogBase.h"

class CMyD3DApplication;



class ParamDialog : public CxxDialogBase
{
	friend class CMyD3DApplication;
	void SetGui_CameraOrbitDegreeLive(float degree);

public:
	ParamDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void DataFromGui();

	void InitParams();

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnClose(HWND hwnd);

	// Chj added members:

private:
//	TScalableArray<> m_saLiveUic;

	DataXIni m_xini;

private:
	D3DLIGHTTYPE m_lighttype;
	LiveUicXString<CRadioGroup, int> mc_LightType; // mc: member of Uic type
	bool m_isPointLightLatitude;

	float m_PointLightHeight; // const now

	LiveUicXString<CEditValue<float>, float> mc_PointLightRadius;

	LiveUicXString<CCheckbox, int> mc_LightAnimation;
	LiveUicXString<CCheckbox, int> mc_CameraAnimation;

	CEditStr mc_LightXYZ; // UI-readonly info

	LiveUicXString<CEditValue<float>, float> mc_CameraHeight;
	LiveUicXString<CEditValue<float>, float> mc_CameraDistance;
	LiveUicXString<CEditValue<float>, float> mc_CameraOrbitDegree;
//	float m_CameraOrbitDegreeLive;
	LiveUicXString<CEditValue<float>, float> mc_CameraWaggleDegreeMax;

	LiveUicXString<CCheckbox, int> mc_IsBackWallGrid;
	LiveUicXString<CCheckbox, int> mc_IsBottomGrid;
	LiveUicXString<CCheckbox, int> mc_IsRightWallGrid;
	LiveUicXString<CCheckbox, int> mc_IsBallGrid;

	LiveUicXString<CRadioGroup, int> mc_Zbuffer;
	LiveUicXString<CRadioGroup, int> mc_Cullmode;

	LiveUicXString<CEditStr, Sdring> mc_edtComment;
};

