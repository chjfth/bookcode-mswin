#pragma once

#include "CxxRawWinBase.h"

class CAppWin : public CxxRawWinBase
{
public:
	virtual BOOL RegisterAndCreate(const TCHAR *szAppName);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT);
	void Cls_OnPaint(HWND hwnd);
	void Cls_OnDestroy(HWND hwnd);

	void Cls_OnCommand(HWND hwnd, int cmdid, HWND hwndCtl, UINT codeNotify);

	void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy);
	void Cls_OnMove(HWND hwnd, int x, int y);
	void Cls_OnTimer(HWND hwnd, UINT id);

	void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

	void Cls_OnInitMenuPopup(HWND hwnd, HMENU hmenuPopup, UINT item, BOOL fSystemMenu);
	void Cls_OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hSubmenu, UINT flags);

	void Cls_OnDropFiles(HWND hwnd, HDROP hdrop);

private:
	void OnWinMoveSize(HWND hwnd);
	void My_OnMouseLeave(HWND hwnd);

	void winenv_ReloadSetting(HWND hwnd);

private:
	// private program states here.
};

