//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : checkmark.cpp						                                 //
//  Description: Using DDB as menu checkmarks                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "CheckMark.h"


void KCheckMark::AddBitmap(int id, HBITMAP hBmp)
{
	if ( m_nUsed < MAXSUBIMAGES )
	{
		m_nSubImageId[m_nUsed]   = id;
		m_hSubImage  [m_nUsed++] = hBmp;
	}
}


void KCheckMark::LoadToolbar(HMODULE hModule, int resid, bool transparent)
{
	// Chj: Load the ribbon image as a whole

	m_hBmp = (HBITMAP) ::LoadImage(hModule, MAKEINTRESOURCE(resid), IMAGE_BITMAP, 0, 0, 
		transparent ? LR_LOADTRANSPARENT : 0);
	AddBitmap((int) hModule + resid, m_hBmp);	// not to be reused as subimage
}


KCheckMark::~KCheckMark()
{
	for (int i=0; i<m_nUsed; i++)
		DeleteObject(m_hSubImage[i]);
}


HBITMAP KCheckMark::GetSubImage(int id)
{
	if ( id < 0 )
		return NULL;

	for (int i=0; i<m_nUsed; i++)
		if ( m_nSubImageId[i]==id )
			return m_hSubImage[i];

	int idx = id;

	// Chj: Now we extract a tiny image(by idx, 0,1,2...) from the ribbon

	BITMAP bmp;

	if ( ! GetObject(m_hBmp, sizeof(bmp), & bmp) )
		return NULL;

	if ( idx*bmp.bmHeight >= bmp.bmWidth )
		return NULL;

	HDC hMemDCS = CreateCompatibleDC(NULL);
	HDC hMemDCD = CreateCompatibleDC(NULL);

	SelectObject(hMemDCS, m_hBmp);

	int w = GetSystemMetrics(SM_CXMENUCHECK);
	int h = GetSystemMetrics(SM_CYMENUCHECK);
	
	HBITMAP hbmpRslt = CreateCompatibleBitmap(hMemDCS, w, h);

	if  ( hbmpRslt )
	{
		HGDIOBJ hOld = SelectObject(hMemDCD, hbmpRslt);
		StretchBlt(hMemDCD, 0, 0, w, h, 
			hMemDCS, idx*bmp.bmHeight, 0, bmp.bmHeight, bmp.bmHeight, 
			SRCCOPY);
		SelectObject(hMemDCD, hOld); // Chj: just to *release* hbmpRslt from the DC, 
		                             // so that deleting this DC will not destroy hbmpRslt?

		AddBitmap(idx, hbmpRslt); // Chj: Cache the HBITMAP for faster re-use,
		                          // and, we can delete this HBITMAP in dtor.
	}

	DeleteObject(hMemDCS);
	DeleteObject(hMemDCD);

	return hbmpRslt;
}


BOOL KCheckMark::SetCheckMarks(HMENU hMenu, UINT uPos, UINT uFlags, int unchecked, int checked)
{
	return SetMenuItemBitmaps(hMenu, uPos, uFlags, GetSubImage(unchecked), GetSubImage(checked));
}

