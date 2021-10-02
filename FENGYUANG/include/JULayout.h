/******************************************************************************
Module:  UILayout.h
Notices: Copyright (c) 2000 Jeffrey Richter
Purpose: This class manages child window positioning and sizing when a parent 
         window is resized.
         See Appendix B.
[2017-06-18] Updated by Chj, renamed to JULayout.
******************************************************************************/

#ifndef __JULayout_h_
#define __JULayout_h_

///////////////////////////////////////////////////////////////////////////////


//#include "..\CmnHdr.h"                 // See Appendix A.


///////////////////////////////////////////////////////////////////////////////

#define JULAYOUT_MAX_CONTROLS 200 

class JULayout 
{
public:
	JULayout();

	bool Initialize(HWND hwndParent, int nMinWidth = 0, int nMinHeight = 0);
	// -- old style

	static JULayout* EnableJULayout(HWND hwndParent);
		// A new JULayout object is returned to caller, and the lifetime of this object
		// is managed automatically, i.e. the JULayout object is destroyed when the
		// window by HWND is destroyed by the system.

	static JULayout* GetJULayout(HWND hwndParent);
   
	// Anco: Anchor coefficient, this value should be a percent value 0~100, 
	// 0 means left-most or top-most, 100 means right-most or bottom most.
	bool AnchorControl(int x1Anco, int y1Anco, int x2Anco, int y2Anco, int nCtrlID, bool fRedraw=false);
	bool AnchorControls(int x1Anco, int y1Anco, int x2Anco, int y2Anco, bool fRedraw, ...);

	bool AdjustControls(int cx, int cy);

	void HandleMinMax(PMINMAXINFO pMinMax) 
	{ 
		pMinMax->ptMinTrackSize = m_ptMinParentDims; 
	}

	static bool EnableForPrsht(HWND hwndPrsht);

private:
	struct Ancofs_st {
		int Anco; // Anchor coefficient, this value should be a percent value 0~100, 
		int Offset; // pixel offset added to Anco-baseline
	};

	struct CtrlInfo_st {
		int         m_nID; 
		BOOL        m_fRedraw;
		Ancofs_st pt1x, pt1y; // pt1 means the north-west corner of the control
		Ancofs_st pt2x, pt2y; // pt2 means the south-east corner of the control
	}; 

	bool PatchWndProc();

	static void PixelFromAnchorPoint(int cxParent, int cyParent, int xAnco, int yAnco, PPOINT ppt)
	{
		ppt->x = cxParent*xAnco/100;
		ppt->y = cyParent*yAnco/100;
	}

private:
	static LRESULT CALLBACK JulWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK PrshtWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:    
	CtrlInfo_st m_CtrlInfo[JULAYOUT_MAX_CONTROLS]; // Max controls allowed in a dialog template
	int     m_nNumControls;
	HWND    m_hwndParent;
	POINT   m_ptMinParentDims; 

	WNDPROC m_prevWndProc;
}; 


///////////////////////////////////////////////////////////////////////////////


#ifdef JULAYOUT_IMPL

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>

#include "dbgprint.h" // temp debug

#define JULAYOUT_STR _T("JULayout")
#define JULAYOUT_PRSHT_STR _T("JULayout.Prsht")
	// Will use these strings to call SetProp()/GetProp(),
	// to associate JULayout object with an HWND.

JULayout::JULayout()
{
	ZeroMemory(m_CtrlInfo, sizeof(m_CtrlInfo));
	m_nNumControls = 0;
	m_hwndParent = NULL;
	m_ptMinParentDims.x = m_ptMinParentDims.y = 0;
	m_prevWndProc = NULL;
}

bool JULayout::Initialize(HWND hwndParent, int nMinWidth, int nMinHeight) 
{
	// User should call this from within WM_INITDIALOG.

	if(!IsWindow(hwndParent))
		return false;

	m_hwndParent = hwndParent;
	m_nNumControls = 0;

	if ((nMinWidth == 0) || (nMinHeight == 0)) {
		RECT rc;
		GetWindowRect(m_hwndParent, &rc);
		m_ptMinParentDims.x = rc.right  - rc.left; 
		m_ptMinParentDims.y = rc.bottom - rc.top; 
	}
	if (nMinWidth  != 0) 
		m_ptMinParentDims.x = nMinWidth;
	if (nMinHeight != 0) 
		m_ptMinParentDims.y = nMinHeight; 

	return true;
}

JULayout* JULayout::GetJULayout(HWND hwndParent)
{
	if(!IsWindow(hwndParent))
		return NULL;

	JULayout *jul = (JULayout*)GetProp(hwndParent, JULAYOUT_STR);
	return jul;
}

JULayout* JULayout::EnableJULayout(HWND hwndParent)
{
	if(!IsWindow(hwndParent))
		return NULL;

	// First check whether a JULayout object has been associated with hwndParent.
	// If so, just return that associated object.
	JULayout *jul = GetJULayout(hwndParent);
	if(jul)
		return jul;

	jul = new JULayout();
	if(!jul)
		return NULL;
	
	bool succ = jul->Initialize(hwndParent);
	if(!succ)
	{
		delete jul;
		return NULL;
	}

	SetProp(hwndParent, JULAYOUT_STR, (HANDLE)jul);

	jul->PatchWndProc();

	return jul;
}

bool JULayout::PatchWndProc()
{
	// Patch WndProc so that we can handle WM_SIZE and WM_GETMINMAX automatically.

	if(!IsWindow(m_hwndParent))
		return false;

	m_prevWndProc = SubclassWindow(m_hwndParent, JulWndProc);

	return true;
}


LRESULT CALLBACK 
JULayout::JulWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JULayout *jul = (JULayout*)GetProp(hwnd, JULAYOUT_STR);
	assert(jul);

	if(msg==WM_SIZE)
	{
		jul->AdjustControls(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if(msg==WM_GETMINMAXINFO)
	{
		jul->HandleMinMax((MINMAXINFO*)lParam);
	}
	else if(msg==WM_DESTROY)
	{
		delete jul;
	}

	LRESULT ret = jul->m_prevWndProc(hwnd, msg, wParam, lParam);
	return ret;
}

struct JULPrsht_st
{
	WNDPROC orig_winproc;
	HWND hwndSystab; // the HWND of the only SysTabControl32 in the Prsht
};

bool JULayout::EnableForPrsht(HWND hwndPrsht)
{
	//
	// Subclass hwndPrsht for WM_SIZE processing
	//

	JULPrsht_st *jprsht = (JULPrsht_st*)GetProp(hwndPrsht, JULAYOUT_PRSHT_STR);
	if(jprsht)
	{
		// EnableForPrsht already called.
		return false; 
	}

	jprsht = new JULPrsht_st;
	jprsht->orig_winproc = SubclassWindow(hwndPrsht, PrshtWndProc);

	SetProp(hwndPrsht, JULAYOUT_PRSHT_STR, jprsht);

	// Make Prsht dialog resizable (pending, not dragable yet, need AmHotkey Ctrl+Win+arrow)
	//
	UINT ostyle = GetWindowStyle(hwndPrsht);
	SetWindowLong(hwndPrsht, GWL_STYLE, (ostyle) | WS_THICKFRAME);

	JULayout *jul = JULayout::EnableJULayout(hwndPrsht);

	// Find child windows of the Prsht and anchor them.
	HWND hwndPrevChild = NULL;
	for(; ;)
	{
		HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);

		if(hwndNowChild==NULL)
			break;

		UINT id = GetWindowID(hwndNowChild);
		TCHAR classname[100] = {};
		GetClassName(hwndNowChild, classname, 100);
		dbgprint("See id=0x08%X , class=%s", id, classname);

		if(_tcsicmp(classname, _T("button"))==0)
		{
			// Meet the bottom-right buttons like OK, Cancel, Apply.
			jul->AnchorControl(100,100, 100,100, id, true);
		}
		else if(_tcsicmp(classname, _T("SysTabControl32"))==0)
		{
			jprsht->hwndSystab = hwndNowChild;
			jul->AnchorControl(0,0, 100,100, id, false);
		}
		else
		{
			// The SysTabControl32 and those #32770 true dialogbox.
			jul->AnchorControl(0,0, 100,100, id, false);
		}

		hwndPrevChild = hwndNowChild;
	}

	return true;
}

LRESULT CALLBACK JULayout::PrshtWndProc(HWND hwndPrsht, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JULPrsht_st *jprsht = (JULPrsht_st*)GetProp(hwndPrsht, JULAYOUT_PRSHT_STR);
	assert(jprsht);
	assert(jprsht->hwndSystab);

	if(msg==WM_SIZE)
	{
		dbgprint("Prsht sizing: %d * %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

		RECT rcSystab;
		GetClientRect(jprsht->hwndSystab, &rcSystab); 
		// -- this RECT changes due to JULayout applied on Prsht.
		
		HWND hwndPrevChild = NULL;
		for(; ;)
		{
			HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);
			if(hwndNowChild==NULL)
				break;
			
			TCHAR classname[100] = {};
			GetClassName(hwndNowChild, classname, 100);
//			dbgprint("See id=0x08%X , class=%s", id, classname);

			if(_tcsicmp(classname, _T("#32770"))==0)
			{
				RECT rc = {};
				GetClientRect(hwndNowChild, &rc);
				MapWindowPoints(hwndNowChild, hwndPrsht, (POINT*)&rc, 2);

				MoveWindow(hwndNowChild, rc.left, rc.top, 
					rcSystab.right -8, 
					rcSystab.bottom-25, // TODO: make -8 & -25 accurate
					TRUE);
			}
			
			hwndPrevChild = hwndNowChild;
		}
	}
	else if(msg==WM_DESTROY)
	{
		delete jprsht;
	}

	LRESULT ret = jprsht->orig_winproc(hwndPrsht, msg, wParam, lParam);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////


bool JULayout::AnchorControl(int x1Anco, int y1Anco, int x2Anco, int y2Anco, int nID, bool fRedraw) 
{
	if(m_nNumControls>=JULAYOUT_MAX_CONTROLS)
		return false;

	HWND hwndControl = GetDlgItem(m_hwndParent, nID);
	if(hwndControl == NULL) 
		return false;
	
	CtrlInfo_st &cinfo = m_CtrlInfo[m_nNumControls];

	cinfo.m_nID = nID;
	cinfo.m_fRedraw = fRedraw;

	cinfo.pt1x.Anco = x1Anco; cinfo.pt1y.Anco = y1Anco;
	cinfo.pt2x.Anco = x2Anco; cinfo.pt2y.Anco = y2Anco; 

	RECT rcControl;
	GetWindowRect(hwndControl, &rcControl);  // Screen coords of control
	// Convert coords to parent-relative coordinates
	MapWindowPoints(HWND_DESKTOP, m_hwndParent, (PPOINT) &rcControl, 2);

	RECT rcParent; 
	GetClientRect(m_hwndParent, &rcParent);

	POINT pt; 
	PixelFromAnchorPoint(rcParent.right, rcParent.bottom, x1Anco, y1Anco, &pt);
	cinfo.pt1x.Offset = rcControl.left - pt.x; 
	cinfo.pt1y.Offset = rcControl.top - pt.y;

	PixelFromAnchorPoint(rcParent.right, rcParent.bottom, x2Anco, y2Anco, &pt);
	cinfo.pt2x.Offset = rcControl.right - pt.x;
	cinfo.pt2y.Offset = rcControl.bottom - pt.y;

	m_nNumControls++;
	return true;
}

bool JULayout::AnchorControls(int x1Anco, int y1Anco, int x2Anco, int y2Anco, bool fRedraw, ...) 
{
	bool fOk = true;

	va_list arglist;
	va_start(arglist, fRedraw);
	int nID = va_arg(arglist, int);
	while (fOk && (nID != -1)) 
	{
		fOk = fOk && AnchorControl(x1Anco, y1Anco, x2Anco, y2Anco, nID, fRedraw);
		nID = va_arg(arglist, int);
	}           
	va_end(arglist);
	return(fOk);
}

bool JULayout::AdjustControls(int cx, int cy) 
{
	bool fOk = false;

	// Create region consisting of all areas occupied by controls
	HRGN hrgnPaint = CreateRectRgn(0, 0, 0, 0);
	int i;
	for(i=0; i<m_nNumControls; i++) 
	{
		HWND hwndControl = GetDlgItem(m_hwndParent, m_CtrlInfo[i].m_nID);
		RECT rcControl; 
		GetWindowRect(hwndControl, &rcControl);  // Screen coords of control
		// Convert coords to parent-relative coordinates
		MapWindowPoints(HWND_DESKTOP, m_hwndParent, (PPOINT) &rcControl, 2);

		HRGN hrgnTemp = CreateRectRgnIndirect(&rcControl);
		CombineRgn(hrgnPaint, hrgnPaint, hrgnTemp, RGN_OR);
		DeleteObject(hrgnTemp);
	}

	for(i=0; i<m_nNumControls; i++) 
	{
		CtrlInfo_st &cinfo = m_CtrlInfo[i];

		// Get control's upper/left position w/respect to parent's width/height
		RECT rcControl; 
		PixelFromAnchorPoint(cx, cy, cinfo.pt1x.Anco, cinfo.pt1y.Anco, (PPOINT)&rcControl);
		rcControl.left += cinfo.pt1x.Offset; 
		rcControl.top  += cinfo.pt1y.Offset; 

		// Get control's lower/right position w/respect to parent's width/height
		PixelFromAnchorPoint(cx, cy, cinfo.pt2x.Anco, cinfo.pt2y.Anco, (PPOINT)&rcControl.right);
		rcControl.right  += cinfo.pt2x.Offset;
		rcControl.bottom += cinfo.pt2y.Offset;

		// Position/size the control
		HWND hwndControl = GetDlgItem(m_hwndParent, cinfo.m_nID);
		MoveWindow(hwndControl, rcControl.left, rcControl.top, 
			rcControl.right - rcControl.left, 
			rcControl.bottom - rcControl.top, FALSE);
		
		if (m_CtrlInfo[i].m_fRedraw) {
			InvalidateRect(hwndControl, NULL, FALSE);
		} else {
			// Remove the regions occupied by the control's new position
			HRGN hrgnTemp = CreateRectRgnIndirect(&rcControl);
			CombineRgn(hrgnPaint, hrgnPaint, hrgnTemp, RGN_DIFF);
			DeleteObject(hrgnTemp);
			// Make the control repaint itself
			InvalidateRect(hwndControl, NULL, TRUE);
			SendMessage(hwndControl, WM_NCPAINT, 1, 0);
			UpdateWindow(hwndControl);
		}
	}

	// Paint the newly exposed portion of the dialog box's client area
	HDC hdc = GetDC(m_hwndParent);
	HBRUSH hbrColor = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	FillRgn(hdc, hrgnPaint, hbrColor);
	DeleteObject(hbrColor);
	ReleaseDC(m_hwndParent, hdc);
	DeleteObject(hrgnPaint);
	return(fOk);
}

#endif   // JULAYOUT_IMPL

///////////////////////////////////////////////////////////////////////////////


#endif // __JULayout_h_
