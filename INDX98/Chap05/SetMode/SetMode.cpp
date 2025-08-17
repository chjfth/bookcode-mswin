#define WIN32_LEAN_AND_MEAN
// You must include this define to use QueryInterface
#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <ddraw.h>

#include "resource.h"

LPDIRECTDRAW            lpDD = NULL;           // DirectDraw object.
LPDIRECTDRAW2           lpDD2 = NULL;          // DirectDraw2 object.
HINSTANCE               hInst;

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

BOOL WINAPI EnumDDrawDevice( GUID FAR *lpGUID,           
                             LPSTR lpDriverDescription,  
                             LPSTR lpDriverName,         
                             LPVOID lpContext )
{
    LONG    iIndex;
    HWND    hWnd = ( HWND )lpContext;
    LPVOID  lpDevice = NULL;

    iIndex = SendMessage( hWnd, CB_ADDSTRING, 0, 
                          ( LPARAM )lpDriverDescription );

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
            if ( !lpDevice ) return FALSE;
            memcpy( lpDevice, lpGUID, sizeof( GUID ) );
        }

        SendMessage( hWnd, CB_SETITEMDATA, iIndex, 
                     ( LPARAM )lpDevice );
    }
    else 
    {
        return DDENUMRET_CANCEL;
    }

    return DDENUMRET_OK;
}

BOOL WINAPI EnumDisplayModes( LPDDSURFACEDESC lpDDSurfaceDesc,  
                              LPVOID lpContext )
{
    LONG    iIndex;
    char    buff[256];
    HWND    hWnd = ( HWND )lpContext;
    LPVOID  lpDesc = NULL;

    wsprintf( buff, "%dx%dx%dx%d", 
              lpDDSurfaceDesc->dwWidth,
              lpDDSurfaceDesc->dwHeight,
              lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount,
              lpDDSurfaceDesc->dwRefreshRate );

    iIndex = SendMessage( hWnd, LB_ADDSTRING, 0, 
                          ( LONG )( LPSTR )buff );

    // If it got added to the list box, create a copy of the
    // surface description and store a pointer to it in the
    // list box. We'll use it later to set the mode.

    if ( iIndex != LB_ERR )
    {
        lpDesc = ( LPDDSURFACEDESC )malloc( sizeof( DDSURFACEDESC ) );
        if ( !lpDesc ) return FALSE;

        memcpy( lpDesc, lpDDSurfaceDesc, sizeof( DDSURFACEDESC ) );

        SendMessage( hWnd, LB_SETITEMDATA, iIndex, 
                     ( LPARAM )lpDesc );
    }
    else 
    {
        return DDENUMRET_CANCEL;
    }

    return DDENUMRET_OK;
}

LRESULT CALLBACK DlgModeProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int                 iIndex;
    LPDDSURFACEDESC     lpDesc = NULL;
    LPGUID              lpDevice = NULL;
    LPVOID              lpHeap = NULL;
    LPDELETEITEMSTRUCT  lpdis = NULL;

    switch ( message )
    {
        case WM_INITDIALOG:
            // Enumerate the DirectDraw devices.
            if ( FAILED( DirectDrawEnumerate( ( LPDDENUMCALLBACK )EnumDDrawDevice,  
                         ( LPVOID )GetDlgItem( hWnd, IDC_DEVICE ) ) ) )
			{
                OutputDebugString( "Couldn't enumerate devices.\n" );
                return FALSE;
            }

            SendDlgItemMessage( hWnd, IDC_DEVICE, 
                                CB_SETCURSEL, 0, 0L );

            return TRUE;

        case WM_COMMAND:
            switch ( LOWORD( wParam ) )
            {
                case IDC_CREATE:
                    // Get the unique id for the selected device. If it's
                    // the primary device, the id will be NULL.
                    iIndex = SendDlgItemMessage( hWnd, IDC_DEVICE, 
                                                 CB_GETCURSEL, 0, 0L );
                    lpDevice = ( LPGUID )SendDlgItemMessage( 
                                             hWnd, IDC_DEVICE, 
                                             CB_GETITEMDATA, iIndex, 0 );
                    // Create the DirectDraw object.
                    if ( FAILED( DirectDrawCreate( lpDevice, &lpDD, NULL ) ) )
					{
                        return Fail( hWnd, "Couldn't create DirectDraw object.\n" );
                    }

                    // Query the appropriate interface.
                    if ( FAILED( lpDD->QueryInterface( IID_IDirectDraw2, 
                                                       ( LPVOID * )&lpDD2 ) ) )
					{
                        return Fail( hWnd, "Couldn't query the interface.\n" );
                    }

                    // Release the interface we don't need.
                    lpDD->Release();

                    /* 
                    
                    // This code can be used to create the DirectDraw
                    // object using CoCreateInstance instead of
                    // DirectDrawCreate.
                    
                    CoInitialize( NULL );

                    if ( FAILED( CoCreateInstance( CLSID_DirectDraw,
                         NULL, CLSCTX_ALL, IID_IDirectDraw2,
                         ( LPVOID* ) &lpDD2 ) ) )
				    {
                        return Fail( hWnd, "Couldn't create DirectDraw object.\n" );
                    }

                    if ( FAILED( lpDD2->Initialize( lpDevice ) ) )
					{
                        return Fail( hWnd, "Couldn't initialize DirectDraw.\n" );
                    }

                    CoUninitialize();

                    */

                    // Set the cooperative level. Give us the
                    // ability to change the bit depth, and don't
                    // fiddle with our window.
                    if ( FAILED( lpDD2->SetCooperativeLevel( hWnd,
                                 DDSCL_FULLSCREEN |
                                 DDSCL_EXCLUSIVE |
                                 DDSCL_NOWINDOWCHANGES ) ) )
					{
                        return Fail( hWnd, "Couldn't set cooperative level.\n" );
                    }

                    // Enumerate the available modes.
                    if ( FAILED( lpDD2->EnumDisplayModes( 0, NULL,
                                 ( LPVOID )GetDlgItem( hWnd, IDC_MODES ),
                                 ( LPDDENUMMODESCALLBACK )EnumDisplayModes ) ) )
					{
                        return Fail( hWnd, "Couldn't enumerate modes.\n" );
                    }
 
                    SendDlgItemMessage( hWnd, IDC_MODES, 
                                        LB_SETCURSEL, 0, 0L );

                    EnableWindow( GetDlgItem( hWnd,IDC_CREATE ), FALSE );
                    EnableWindow( GetDlgItem( hWnd,IDC_SET ), TRUE );

                    break;

                case IDC_SET:
                    // Get the surface description referenced in the list box.
                    iIndex = SendDlgItemMessage( hWnd, IDC_MODES, 
                                                 LB_GETCURSEL, 0, 0L );
                    lpDesc = ( LPDDSURFACEDESC )SendDlgItemMessage( 
                                                    hWnd, IDC_MODES, 
                                                    LB_GETITEMDATA, iIndex, 0 );

                    // Set the new display mode.
                    if ( FAILED( lpDD2->SetDisplayMode( lpDesc->dwWidth, 
                                          lpDesc->dwHeight,       
                                          lpDesc->ddpfPixelFormat.dwRGBBitCount,
                                          lpDesc->dwRefreshRate, 0 ) ) )
					{
                        return Fail( hWnd, "Couldn't set the mode.\n");
                    }
                    break;

                case IDCANCEL:
                    EndDialog( hWnd, FALSE );
                    return TRUE;
            }
            break;

       case WM_DELETEITEM:
            // A clean way to free the memory allocated
            // when the list and combo boxes are filled.
            // Don't use this technique on NT -- the
            // message isn't sent.
            lpdis = ( LPDELETEITEMSTRUCT )lParam;
            lpHeap = ( LPVOID )lpdis->itemData;
            if ( lpHeap ) 
			{
                free( lpHeap );
            }
            return TRUE;

        case WM_DESTROY:
            break;

    }
    return FALSE; 

}


/*
 * WindowProc
 *
 * Messages for our window are handled here
 */
LRESULT CALLBACK WindowProc( HWND hwnd, unsigned uMsg, WPARAM wParam, LPARAM lParam )
{
    switch ( uMsg )
    {
        case WM_PAINT:
            PAINTSTRUCT     ps;

            BeginPaint( hwnd, &ps );
            EndPaint( hwnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
    }
    return 0L;
}

static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    hInst = hInstance;

    return TRUE;                       
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    BOOL rc;

    if ( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    rc = DialogBox( hInst, MAKEINTRESOURCE( IDD_MAIN ), NULL, ( DLGPROC )DlgModeProc );

    return 0;
}
