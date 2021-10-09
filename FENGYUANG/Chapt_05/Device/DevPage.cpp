//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : devpage.cpp			                                             //
//  Description: KDevicePage class                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define WINVER       0x0500

#include <windows.h>
#include <tchar.h>

#include "resource.h"

#include "..\..\include\property.h"
#include "..\..\include\listview.h"
#include "..\..\include\utils.h"

#include "DevPage.h"
#include "DevCaps.h"
#include "DCAttr.h"

void AddDisplayDevices(HWND hList)
{
	DISPLAY_DEVICE Dev;

	Dev.cb = sizeof(Dev);

	SendMessage(hList, CB_RESETCONTENT, 0, 0);

	for (unsigned i=0; 
	     EnumDisplayDevices(NULL, i, &Dev, 0);
		 i++ )
	{
		// [output] Dev.DeviceName is like:
		//	\\.\DISPLAY1
		//	\\.\DISPLAY2
		SendMessage(hList, CB_ADDSTRING, 0, (LPARAM) Dev.DeviceName);
	}

 	SendMessage(hList, CB_SETCURSEL, 0, 0);
}


int FrameBufferSize(int width, int height, int bpp)
{
	int bytepp = ( bpp + 7 ) / 8;	             // bytes per pixel
	int byteps = ( width * bytepp + 3 ) / 4 * 4; // bytes per scanline

	return height * byteps;						 // bytes per frame buffer
}


void AddDisplaySettings(HWND hList, LPCTSTR pszDeviceName)
{
	DEVMODE  dm;

	dm.dmSize		 = sizeof(DEVMODE);
	dm.dmDriverExtra = 0;

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	for (unsigned i=0;
	     EnumDisplaySettings(pszDeviceName, i, & dm);
		 i++ )
	{
		TCHAR szTemp[MAX_PATH];

		wsprintf(szTemp, _T("%d by %d, %d bits, %d Hz, %d KB"), 
			dm.dmPelsWidth, 
			dm.dmPelsHeight, 
			dm.dmBitsPerPel,
			dm.dmDisplayFrequency,
			FrameBufferSize(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel) / 1024
			);

		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) szTemp);
	}
}


BOOL KDevicePage::OnInitDialog(HWND hWnd)
{
	AddDisplayDevices(GetDlgItem(hWnd, IDC_DEVICE));

	OnDeviceChange(hWnd);

	JULayout *jul = JULayout::EnableJULayout(hWnd);
	jul->AnchorControls(0,0, 100,0,  
		IDC_DEVICE, IDC_DEVICESTRING, IDC_DEVICEID, IDC_DEVICEKEY, IDC_STATEFLAGS,
		IDC_TEXT_iDevNum, IDC_TEXT_StateFlags
		);

	jul->AnchorControls(0,0, 100,100, IDC_MODES);

    return TRUE;
}


BOOL KDevicePage::OnDeviceChange(HWND hWnd)
{
	TCHAR szDeviceName[MAX_PATH];

	GetDlgItemText(hWnd, IDC_DEVICE, szDeviceName, MAX_PATH);
	
	{
		unsigned iDevNum = 0; 
		DISPLAY_DEVICE Device;

		Device.cb = sizeof(Device);

		while ( EnumDisplayDevices(NULL, iDevNum, & Device, 0) )
		{
			// Dev.DeviceName is like:
			//	\\.\DISPLAY1
			//	\\.\DISPLAY2

			if ( _tcscmp(szDeviceName, Device.DeviceName)==0 )
				break;
			else
				iDevNum ++;
		}

		vaSetDlgItemText(hWnd, IDC_TEXT_iDevNum, _T("iDevNum = %d"), iDevNum);

		SetDlgItemText(hWnd, IDC_DEVICESTRING, Device.DeviceString); 
		// -- e.g. "VirtualBox Graphics Adapter (WDDM)"
		
		SetDlgItemText(hWnd, IDC_DEVICEID,     Device.DeviceID); 
		// -- e.g. "PCI\VEN_80EE&DEV_BEEF&SUBSYS_040515AD&REV_00"
		
		SetDlgItemText(hWnd, IDC_DEVICEKEY,    Device.DeviceKey); 
		// -- e.g. "\Registry\Machine\System\CurrentControlSet\Control\Video\{F53FDD4F-2C23-4245-B138-B1667CB4F52F}\0000"

		vaSetDlgItemText(hWnd, IDC_TEXT_StateFlags, "StateFlags = 0x%08X", Device.StateFlags);

		SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_RESETCONTENT, 0, 0);
					
		if ( Device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "ATTACHED_TO_DESKTOP");

		if ( Device.StateFlags & DISPLAY_DEVICE_MULTI_DRIVER )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "MULTI_DRIVER");

		if ( Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "PRIMARY_DEVICE");

		if ( Device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "MIRRORING_DRIVER");

		if ( Device.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "VGA_COMPATIBLE");

		if ( Device.StateFlags & DISPLAY_DEVICE_REMOTE )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "REMOTE");

		if ( Device.StateFlags & DISPLAY_DEVICE_DISCONNECT )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "DISCONNECT");

		if ( Device.StateFlags & DISPLAY_DEVICE_TS_COMPATIBLE )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "TS_COMPATIBLE");

		if ( Device.StateFlags & DISPLAY_DEVICE_MODESPRUNED )
			SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_ADDSTRING, 0, (LPARAM) "MODESPRUNED");

		SendDlgItemMessage(hWnd, IDC_STATEFLAGS, CB_SETCURSEL, 0, 0);
	}

	AddDisplaySettings(GetDlgItem(hWnd, IDC_MODES), szDeviceName);

	return TRUE;
}



BOOL KDevicePage::OnDeviceCaps(HWND hWnd)
{
	TCHAR szDeviceName[MAX_PATH];

	GetDlgItemText(hWnd, IDC_DEVICE, szDeviceName, MAX_PATH);

	HDC hDC = CreateDC(_T("DISPLAY"), szDeviceName, NULL, NULL);

	KDeviceCaps caps(hDC);

	caps.Dialogbox(m_hInstance, MAKEINTRESOURCE(IDD_DEVICECAPS));
	
	DeleteDC(hDC);

	return TRUE;
}


BOOL KDevicePage::OnDCAttributes(HWND hWnd)
{
	TCHAR szDeviceName[MAX_PATH];

	GetDlgItemText(hWnd, IDC_DEVICE, szDeviceName, MAX_PATH);

	HDC hDC = CreateDC(_T("DISPLAY"), szDeviceName, NULL, NULL);

	KDCAttributes attr(hDC);

	attr.Dialogbox(m_hInstance, MAKEINTRESOURCE(IDD_DCATTRIBUTES), hWnd);
	
	DeleteDC(hDC);

	return TRUE;
}


BOOL KDevicePage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
	    case WM_INITDIALOG:
			return OnInitDialog(hWnd);

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDC_DEVICE:
					if ( HIWORD(wParam)==CBN_SELCHANGE )
						return OnDeviceChange(hWnd);
					break;

				case IDC_DEVICECAPS:
					return OnDeviceCaps(hWnd);

				case IDC_DCATTR:
					return OnDCAttributes(hWnd);
			}
    }
    
    return FALSE;
}
