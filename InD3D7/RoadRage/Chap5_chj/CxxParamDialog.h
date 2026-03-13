#pragma once
#include <windows.h>
#include <d3dtypes.h>

#include "CxxDialogBase.h"

class CMyD3DApplication;

//
// Chj constants of default values
//
#define C_CameraDistance 4.0f
#define C_CameraOrbitDegree -135.0f
#define C_CameraHeight 3.0f



class ParamDialog : public CxxDialogBase
{
	friend class CMyD3DApplication;
	void SetGui_CameraOrbitDegreeLive(float degree);

public:
	ParamDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void GuiToData();

	void ResetParams();

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnClose(HWND hwnd);

private:
	BOOL m_isLightAnimation;
	BOOL m_isCameraAnimation;

	D3DLIGHTTYPE m_lighttype;

	float m_CameraHeight;
	float m_CameraDistance;
	float m_CameraOrbitDegree;
//	float m_CameraOrbitDegreeLive;
	float m_CameraWaggleDegree;
};

