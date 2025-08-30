#define WIN32_LEAN_AND_MEAN

// You must include this define to use QueryInterface(IID_IDirectDraw2)
#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <ddraw.h>
#include "resource.h"

#define utils_env_IMPL
#include <mswin/utils_env.h>

#define utils_wingui_IMPL
#include <mswin/utils_wingui.h>

#define JULayout2_IMPL
#include <mswin/JULayout2.h>

//#include <vaDbg.h>
#include "CxxDialog.h"

#define ExeVersion "0.2"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


class MainDialog : public CxxDialog
{
public:
	MainDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);

private:
	void ReleaseDD()
	{
		if(m_lpDD2) {
			m_lpDD2->Release();
			m_lpDD2 = NULL;
		}
	}

private:
	LPDIRECTDRAW2 m_lpDD2; // todo: make it a Cec
};

MainDialog::MainDialog()
{
	m_lpDD2 = NULL;
}

static BOOL WINAPI 
PROC_EnumDDrawDeviceEx( GUID FAR *lpGUID,           
	LPSTR lpDriverDescription,  LPSTR lpDriverName,         
	LPVOID lpContext, HMONITOR hMonitor )
{
	LONG    iIndex;
	HWND    hdlg = ( HWND )lpContext;
	LPVOID  lpDevice = NULL; // Chj: Better var naming? This refers to a DirectDraw object GUID.

	iIndex = ComboBox_AddString(hdlg, lpDriverDescription);

	// If it got added to the list box, create a copy of the GUID
	// and store a pointer to it in the list box.

	if ( iIndex != LB_ERR )
	{
		// Make sure to check for NULL -- NULL corresponds to the 
		// primary device, which isn't given a GUID.
		if ( lpGUID == NULL ) 
		{
			lpDevice = NULL;
		}
		else
		{
			lpDevice = ( LPGUID )malloc( sizeof( GUID ) );
			if ( !lpDevice ) 
				return FALSE;
			memcpy( lpDevice, lpGUID, sizeof( GUID ) );
		}

		ComboBox_SetItemData(hdlg, iIndex, lpDevice);
	}
	else 
	{
		return DDENUMRET_CANCEL; // =0
	}

	return DDENUMRET_OK; // =1
}


static HRESULT WINAPI 
PROC_EnumDisplayModes( LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext )
{
	LONG    iIndex;
	char    buff[256] = {};
	HWND    hwndListbox = ( HWND )lpContext;
	LPVOID  lpDesc = NULL;

	_snprintf_s( buff, _TRUNCATE, "%dx%dx%dx%d", 
		lpDDSurfaceDesc->dwWidth,
		lpDDSurfaceDesc->dwHeight,
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
		lpDDSurfaceDesc->dwRefreshRate );

	iIndex = ListBox_AddString(hwndListbox, buff);

	// If it got added to the list box, create a copy of the
	// surface description and store a pointer to it in the
	// list box. We'll use it later to set the mode.

	if ( iIndex != LB_ERR )
	{
		lpDesc = ( LPDDSURFACEDESC )malloc( sizeof( DDSURFACEDESC ) );
		if ( !lpDesc ) 
			return FALSE;

		memcpy( lpDesc, lpDDSurfaceDesc, sizeof( DDSURFACEDESC ) );

		ListBox_SetItemData( hwndListbox, iIndex, lpDesc );
	}
	else 
	{
		return DDENUMRET_CANCEL; // =0
	}

	return DDENUMRET_OK; // =1
}


void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	int                 iIndex = 0;
	LPDDSURFACEDESC     lpDesc = NULL;
	LPGUID              lpDevice = NULL;
	HWND                hCombobox = NULL;
	HRESULT             hret = 0;

	switch ( id )
	{{
	case IDC_CREATE:
	{
		// Get the unique id for the selected device. If it's
		// the primary device, the id will be NULL.

		hCombobox = GetDlgItem(hdlg, IDC_DEVICE);
		iIndex = ComboBox_GetCurSel(hCombobox);
			 
		lpDevice = (LPGUID)ComboBox_GetItemData(hCombobox, iIndex);
		
#if 1
		LPDIRECTDRAW        lpDD = NULL;

		// Create the DirectDraw object.
		if ( FAILED( DirectDrawCreate( lpDevice, &lpDD, NULL ) ) )
		{
			// return Fail( hdlg, "Couldn't create DirectDraw object.\n" ); todo
			return;
		}

		// Query the appropriate interface.
		if ( FAILED( lpDD->QueryInterface( IID_IDirectDraw2, (LPVOID*)&m_lpDD2 ) ) )
		{
			assert(!m_lpDD2);
			// return Fail( hdlg, "Couldn't query the interface.\n" ); todo
			return;
		}

		// Release the interface we don't need.
		lpDD->Release();

#else
			
		// This code can be used to create the DirectDraw
		// object using CoCreateInstance() instead of DirectDrawCreate().
					
		hret = CoInitialize( NULL );

		hret = CoCreateInstance( CLSID_DirectDraw,
			NULL, CLSCTX_ALL, IID_IDirectDraw2,
			( LPVOID* ) &m_lpDD2 );

		if ( FAILED(hret) )
		{
			// return Fail( hWnd, "Couldn't create DirectDraw object.\n" ); todo
			return;
		}

		hret = m_lpDD2->Initialize( lpDevice );
		if ( FAILED(hret) )
		{
			// return Fail( hWnd, "Couldn't initialize DirectDraw.\n" ); todo
			return;
		}

		CoUninitialize();

#endif

		// Set the cooperative level. Give us the
		// ability to change the bit depth, and don't
		// fiddle with our window.
		hret = m_lpDD2->SetCooperativeLevel(hdlg,
						DDSCL_FULLSCREEN |
						DDSCL_EXCLUSIVE |
						DDSCL_NOWINDOWCHANGES
						);
		if( FAILED(hret) )
		{
			// return Fail( hWnd, "Couldn't set cooperative level.\n" ); // todo vaDbg
			m_lpDD2->Release();
			m_lpDD2 = NULL;
			return;
		}

		// Enumerate the available modes.
		hret = m_lpDD2->EnumDisplayModes( 0, NULL,
			GetDlgItem( hdlg, IDC_MODES ),
			PROC_EnumDisplayModes);
		if ( FAILED(hret) )
		{
			// return Fail( hWnd, "Couldn't enumerate modes.\n" ); // todo vaDbg
			m_lpDD2->Release();
			m_lpDD2 = NULL;
			return;
		}
 
		SendDlgItemMessage( hdlg, IDC_MODES, 
							LB_SETCURSEL, 0, 0L );

		EnableWindow( GetDlgItem( hdlg,IDC_CREATE ), FALSE );
		EnableWindow( GetDlgItem( hdlg,IDC_SET ), TRUE );

		return;
	}

	case IDC_SET:
		// Get the surface description referenced in the list box.
		iIndex = (int)SendDlgItemMessage( hdlg, IDC_MODES, LB_GETCURSEL, 0, 0L );
		lpDesc = ( LPDDSURFACEDESC )SendDlgItemMessage( 
										hdlg, IDC_MODES, 
										LB_GETITEMDATA, iIndex, 0 );

		// Set the new display mode.
		if ( FAILED( m_lpDD2->SetDisplayMode( lpDesc->dwWidth, 
								lpDesc->dwHeight,       
								lpDesc->ddpfPixelFormat.dwRGBBitCount,
								lpDesc->dwRefreshRate, 0 ) ) )
		{
			// return Fail( hWnd, "Couldn't set the mode.\n"); // todo vaDbg
			// leave m_lpDD2 intact.
			return;
		}
		
		return;

	case IDOK:
		return;

	case IDCANCEL:
		ReleaseDD();
		EndDialog( hdlg, FALSE );
		return;
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

// 	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
// 	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
// 	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(1)));

 	vaSetWindowText(hdlg, _T("SetModeEx v%s (%s)"), _T(ExeVersion), env_GetCpuArch());

	Dlg_EnableJULayout(hdlg);

	// Enumerate all DirectDraw devices.
	HRESULT hret = DirectDrawEnumerateEx( PROC_EnumDDrawDeviceEx,  
		( LPVOID )GetDlgItem( hdlg, IDC_DEVICE ),
		DDENUM_ATTACHEDSECONDARYDEVICES);

	if(FAILED(hret))
	{
//		vaDbgTs("DirectDrawEnumerateEx() fail! hret=0x%08X.", hret); // todo
		return FALSE;
	}

	HWND hCombobox = GetDlgItem(hdlg, IDC_DEVICE);
	ComboBox_SetCurSel(hCombobox, 0); 

	SetFocus(GetDlgItem(hdlg, IDC_CREATE));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR MainDialog::DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

//	const TCHAR *szfullcmdline = GetCommandLine();
//	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	MainDialog dlg;
	dlg.DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_MAIN), NULL);

	return 0;
}


//////////////////////////////////////////////////////////////////////////


/*
static void ReleaseObjects( void )
{
	if ( lpDD2 != NULL )
	{
		lpDD2->Release();
		lpDD2 = NULL;
	}
}

BOOL Fail( HWND hwnd, char *szMsg )
{
	ReleaseObjects();
	OutputDebugString( szMsg );
	EndDialog( hwnd, FALSE );
	return FALSE;
}
*/
