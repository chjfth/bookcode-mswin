#pragma once
#include <windows.h>
#include <d3dtypes.h>

#include <TScalableArray.h>

#include <mswin/LiveUic.h>
using namespace liveuic;

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

private:
	D3DLIGHTTYPE m_lighttype;
	CRadioGroup mc_LightType; // mc: member of Uic type
	bool m_isPointLightLatitude;

	float m_PointLightHeight; // const now
	CEditValue<float> mc_PointLightRadius;

	CCheckbox mc_LightAnimation;
	CCheckbox mc_CameraAnimation;

	CEditStr mc_LightXYZ;

	CEditValue<float> mc_CameraHeight;
	CEditValue<float> mc_CameraDistance;
	CEditValue<float> mc_CameraOrbitDegree;
//	float m_CameraOrbitDegreeLive;
	CEditValue<float> mc_CameraWaggleDegreeMax;

	CCheckbox mc_IsBackWallGrid;
	CCheckbox mc_IsBottomGrid;
	CCheckbox mc_IsRightWallGrid;
	CCheckbox mc_IsBallGrid;

	CRadioGroup mc_Zbuffer;
	CRadioGroup mc_Cullmode;
};

