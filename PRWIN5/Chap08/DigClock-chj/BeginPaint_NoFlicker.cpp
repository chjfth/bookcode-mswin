#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <string.h>
#include <windows.h>
#include "BeginPaint_NoFlicker.h"

// Info: https://docs.microsoft.com/en-us/previous-versions/ms969905(v=msdn.10)

struct DblBuffer_st
{
	HDC hdcReal;
	HDC hdcMem;
	HBITMAP hbmp;

	int cxReal;
	int cyReal;

	int nSaveDC;

} *g_pdbdc = NULL;


HDC BeginPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint)
{
	assert(g_pdbdc==NULL);

	DblBuffer_st *pdb = new DblBuffer_st;
	memset(pdb, 0, sizeof(*pdb));

	pdb->hdcReal = BeginPaint(hwnd, lpPaint);

	pdb->hdcMem = CreateCompatibleDC(pdb->hdcReal);

	RECT rcReal = {};
	GetClientRect(hwnd, &rcReal);
	pdb->cxReal = rcReal.right - rcReal.left;
	pdb->cyReal = rcReal.bottom - rcReal.top;

	pdb->hbmp = CreateCompatibleBitmap(pdb->hdcReal, pdb->cxReal, pdb->cyReal);

	SelectObject(pdb->hdcMem, pdb->hbmp);

	pdb->nSaveDC = SaveDC(pdb->hdcMem);

	g_pdbdc = pdb;
	return pdb->hdcMem;
}

BOOL EndPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint)
{
	assert(g_pdbdc!=NULL);

	DblBuffer_st *pdb = g_pdbdc;

	// Here, we need to restore the pristine DC params for hdcMem,
	// so that LT(0,0), RB(pdb->cxReal,pdb->cyReal) actually refers to the whole client-area.
	// I mean, if the caller had set a non-MM_TEXT mapping mode, 
	// LT(0,0), RB(pdb->cxReal,pdb->cyReal) would refer to a totally different area.
	BOOL succ = RestoreDC(pdb->hdcMem, pdb->nSaveDC);

	succ = BitBlt(pdb->hdcReal, 0, 0, pdb->cxReal, pdb->cyReal, 
		pdb->hdcMem, 0, 0, SRCCOPY);

	EndPaint(hwnd, lpPaint);

	succ = ReleaseDC(hwnd, pdb->hdcReal);

	DeleteDC(pdb->hdcMem);
	DeleteObject(pdb->hbmp);

	delete pdb;
	g_pdbdc = NULL;
	return succ;
}
