//-----------------------------------------------------------------------------
// File: info.cpp
//
// Desc: Code for handling output of program information to the user
//
// Copyright (c) 1999 William Chin and Peter Kovach. All rights reserved.
//-----------------------------------------------------------------------------
#include <tchar.h>
#include "resource.h"
#include <ddraw.h>
#include "d3dframe.h"
#include "D3DApp.h"
#include "roadrage.hpp"

extern CMyD3DApplication* pCMyApp;
extern TCHAR D3Ddevicename[256];

void CMyD3DApplication::DisplayCredits(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	TCHAR  buffer[255];
	HBRUSH holdbrush_color;
	
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
				
	_tcscpy_s(buffer, _T("Credits for Road Rage :"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=40;

	_tcscpy_s(buffer, _T("William Chin"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("Project Leader / C++ Programmer"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;
	
	_tcscpy_s(buffer, _T("Peter Kovach"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("C++ Programmer"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Mark Bracey"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("3DS Modeller / Mapper "));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("NEO XS"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("MD2 Modeller"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("James Glendenning"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("3DS Modeller"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Adam Bardsley"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("Artist / 3DS Modeller"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Correia Emmanuel"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("Artist - textures, website :"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Axem Textues"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("http://axem2.simplenet.com"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=40;

	_tcscpy_s(buffer, _T("Further Info on Road Rage :"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Head Tapper Software"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("http://www.headtappersoftware.com"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	_tcscpy_s(buffer, _T("Email us at:"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	_tcscpy_s(buffer, _T("roadrage@headtappersoftware.com"));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}


void CMyD3DApplication::DisplayRRStats(HWND hwnd)
{
	HDC hdc;
	int mem;	
	int sx1 = 15, sx2 = 320, sy = 70;
	int nIndex;
	TCHAR  buffer[255];
	TCHAR  buf2[255];
	LPTSTR buffer2;
	LPDIRECTDRAW7 lpDD7;
	DDSCAPS2    ddsCaps2; 
	DWORD		total_video_memory;
	DWORD		free_video_memory;
	HRESULT     hr; 
	MEMORYSTATUS memStatus;
	HBRUSH holdbrush_color;

	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;

	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
	memStatus.dwLength=sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memStatus);
		
	mem=(int)memStatus.dwMemoryLoad;
	_itot_s(mem,buffer,10);
	buffer2 = lstrcat(buffer, _T(" %"));
	_tcscpy_s(buf2, _T("System memory in use :"));
	TextOut(hdc, sx1, sy, buf2,    _tcslen(buf2));
	TextOut(hdc, sx2, sy, buffer2, _tcslen(buffer2));
	sy +=20;

	mem=(int)memStatus.dwTotalPhys;
	mem = mem /1024;
	_itot_s(mem, buffer,10);
	_tcscat_s(buffer, _T(" KB"));
	_tcscpy_s(buf2, _T("Total system memory :"));
	TextOut(hdc, sx1, sy, buf2,   _tcslen(buf2));  
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer));
	sy +=20;
						
	hr = GetFramework()->GetDirectDraw()->QueryInterface(IID_IDirectDraw7, (void **)&lpDD7); 
	if (FAILED(hr))
		return; 
 
	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));
  
	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY  ; 
	hr = GetFramework()->GetDirectDraw()->GetAvailableVidMem(&ddsCaps2, &total_video_memory, &free_video_memory); 
	if (FAILED(hr))
		return;

	mem= total_video_memory;
	mem = mem /1024;
	_itot_s(mem,buffer,10);
	_tcscat_s(buffer, _T(" KB"));
	_tcscpy_s(buf2, _T("Total video memory :"));
	TextOut(hdc, sx1, sy, buf2,   _tcslen(buf2));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer)); 
	sy +=20;

	mem= free_video_memory;
	mem = mem /1024;
	_itot_s(mem,buffer,10);
	_tcscat_s(buffer, _T(" KB"));
	_tcscpy_s(buf2, _T("Free video memory :"));
	TextOut(hdc, sx1, sy, buf2,   _tcslen(buf2));
	TextOut(hdc, sx2, sy, buffer, _tcslen(buffer)); 
	sy +=20;

	TextOut(hdc, sx1, sy, _T("D3D Device Name :"),17);
	TextOut(hdc, sx2, sy, D3Ddevicename, _tcslen(D3Ddevicename));
	sy +=20;
	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  
}


void CMyD3DApplication::DisplayLegalInfo(HWND hwnd)
{
	HDC hdc;
	int sx1 = 15, sx2 = 200, sy = 70;
	int nIndex;
	TCHAR  buffer[255];
	HBRUSH holdbrush_color;
	
	hdc=GetDC(hwnd);

	nIndex = COLOR_ACTIVEBORDER;
	holdbrush_color = (HBRUSH)SelectObject(hdc, GetSysColorBrush(nIndex)); 
	Rectangle(hdc,10,58,512,360);

	SetBkMode(hdc, TRANSPARENT);
			
	
	_tcscpy_s(buffer, _T("Legal info for Road Rage :"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=40;

	_tcscpy_s(buffer, _T("The code, 3D models, artwork and sounds in Road Rage,"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=20;
	_tcscpy_s(buffer, _T("are to be used for your own personal use only."));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=40;

	_tcscpy_s(buffer, _T("Anyone wishing to use any part of Road Rage for commercial purposes"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=20;
	_tcscpy_s(buffer, _T("must have the writers / artists written permission first."));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=40;

	_tcscpy_s(buffer, _T("Copyright (C) 1999"));
	TextOut(hdc, sx1, sy, buffer, _tcslen(buffer));
	sy +=20;

	SetBkMode(hdc, OPAQUE);
	ReleaseDC(hwnd,hdc);  

}



BOOL FAR PASCAL AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG:
			SetTimer(hwnd,1,100,NULL);
			break;

		case WM_LBUTTONDOWN:
			break;
			
		case WM_RBUTTONDOWN:
			break;

		case WM_TIMER:
			KillTimer(hwnd,1);
			pCMyApp->DisplayCredits(hwnd);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDOK:
				case IDCANCEL:      
					EndDialog(hwnd, TRUE);
					break;

				case IDC_BUTTON1:
					pCMyApp->DisplayCredits(hwnd);
					break;
				
				case IDC_BUTTON2:
					pCMyApp->DisplayRRStats(hwnd);
					break;

				case IDC_BUTTON4:
					pCMyApp->DisplayLegalInfo(hwnd);	
					break;
			break;
			}
	break;
		
	}

	return FALSE;
}

 
