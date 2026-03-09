#pragma once
#include <windows.h>
#include <d3dtypes.h>

#include "CxxDialog.h"

class CMyD3DApplication;

//
// Chj constants of default values
//
#define C_CameraDistance 4.0f
#define C_CameraSlideDegree 45.f
#define C_CameraHeight 3.0f



class ParamDialog : public CxxDialog
{
	friend class CMyD3DApplication;
public:
	ParamDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void GuiToData(HWND hdlg);

	void ResetParams();

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnClose(HWND hwnd);

private:
	BOOL m_isLightAnimation;
	BOOL m_isCameraAnimation;

	D3DLIGHTTYPE m_lighttype;

	float m_CameraDistance;
	float m_CameraSlideDegree;
	float m_CameraHeight;
	float m_CameraWaggleDegree;
};

