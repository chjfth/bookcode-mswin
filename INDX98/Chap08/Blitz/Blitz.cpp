#define NAME "Blitz"
#define TITLE "DirectDraw Blitting"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

LPDIRECTDRAW            lpDD;               // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;       // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSOffOne;        // DirectDraw offscreen surface
LPDIRECTDRAWSURFACE     lpDDSOffTwo;        // DirectDraw offscreen surface
LPDIRECTDRAWSURFACE     lpDDSOffThree;      // DirectDraw offscreen surface
BOOL                    DestKey = FALSE;    // Destination key capability
DWORD                   dwGreen;            // Pure green
DWORD                   dwBlue;             // Pure blue


BOOL myLoadImage( LPDIRECTDRAWSURFACE lpDDS, LPSTR szImage )
{
    HBITMAP         hbm;
    HDC             hdcImage= NULL;
    HDC             hdcSurf = NULL;
    BOOL            bReturn = FALSE;
    DDSURFACEDESC   ddsd;

    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );

    if ( FAILED( lpDDS->GetSurfaceDesc( &ddsd ) ) )
	{
        goto Exit;
    }

    // If the pixel format isn't some flavor of RGB, we can't handle it.
    if ( ( ddsd.ddpfPixelFormat.dwFlags != DDPF_RGB ) ||
         ( ddsd.ddpfPixelFormat.dwRGBBitCount < 16 ) )

    {
        OutputDebugString( "Non-palettized RGB mode required.\n" );
        goto Exit;        
    }

    // Try loading the image.
    hbm = ( HBITMAP )LoadImage( NULL, szImage, 
            IMAGE_BITMAP, ddsd.dwWidth, 
            ddsd.dwHeight, LR_LOADFROMFILE | LR_CREATEDIBSECTION );

    if ( hbm == NULL ) 
	{
        OutputDebugString( " Couldn't find the resource.\n" );
        goto Exit;
    }

    // Create a DC and select the image into it.
    hdcImage = CreateCompatibleDC( NULL );
    SelectObject( hdcImage, hbm );
   
    // Get a DC for the surface.
    if ( FAILED( lpDDS->GetDC( &hdcSurf ) ) )
	{
        OutputDebugString( "Couldn't get a DC.\n" );
        goto Exit;
    }
    
    // The BitBlt will perform format conversion as necessary.
    if ( BitBlt( hdcSurf, 0, 0, ddsd.dwWidth, ddsd.dwHeight, 
        hdcImage, 0, 0, SRCCOPY ) == FALSE ) 
	{
        OutputDebugString( "Blt failed.\n" );
        goto Exit;
    }

    // Success.
    bReturn = TRUE;
    
Exit:
    // Clean up everything.
    if ( hdcSurf )
        lpDDS->ReleaseDC( hdcSurf );
    if ( hdcImage )
        DeleteDC( hdcImage );
    if( hbm )
        DeleteObject( hbm );

    return bReturn;
}

static void ReleaseObjects( void )
{
    if ( lpDD != NULL )
    {
        if ( lpDDSPrimary != NULL )
        {
            if ( lpDDSOffOne != NULL )
            {
                lpDDSOffOne->Release();
                lpDDSOffOne = NULL;
            }
            if ( lpDDSOffTwo != NULL )
            {
                lpDDSOffTwo->Release();
                lpDDSOffTwo = NULL;
            }
            if ( lpDDSOffThree != NULL )
            {
                lpDDSOffThree->Release();
                lpDDSOffThree = NULL;
            }
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
}

BOOL Fail( HWND hwnd,  char *szMsg )
{
    ReleaseObjects();
	OutputDebugString( szMsg );
    DestroyWindow( hwnd );
    return FALSE;
}

LRESULT FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    DDBLTFX     ddbltfx;    // For blit effects.
    DDCOLORKEY  ddckey;     // For SetColorKey.
    RECT        rcRect;     // For shrinking.

    switch ( message )
    {
		case WM_SETCURSOR:
			SetCursor( NULL );	// Turn off the mouse cursor.
			return TRUE;
 
		case WM_KEYDOWN:
			switch ( wParam )
			{
				case VK_ESCAPE:
					PostMessage( hWnd, WM_CLOSE, 0, 0 );
					break;

                case VK_F1:
                    OutputDebugString( "No color keying...\n" );

                    // Blit the background with stretching.
                    lpDDSPrimary->Blt( NULL, lpDDSOffThree, NULL,
                                       DDBLT_WAIT, NULL );

                    // Blit our sprites without color keys.
                    lpDDSPrimary->BltFast( 300, 200, lpDDSOffOne, NULL,
                                           DDBLTFAST_WAIT | 
										   DDBLTFAST_SRCCOLORKEY );

                    lpDDSPrimary->BltFast( 350, 250, lpDDSOffTwo, NULL,
                                           DDBLTFAST_WAIT |
										   DDBLTFAST_SRCCOLORKEY );
                    break;

                case VK_F2:
                    OutputDebugString( "Source color keying...\n" );

                    // Blit the background with stretching.
                    lpDDSPrimary->Blt( NULL, lpDDSOffThree, NULL,
                                       DDBLT_WAIT, NULL );

                    // Blit our sprites with source color keys.
                    lpDDSPrimary->BltFast( 300, 200, lpDDSOffOne, NULL,
                                           DDBLTFAST_WAIT | 
                                           DDBLTFAST_SRCCOLORKEY );

                    lpDDSPrimary->BltFast( 350, 250, lpDDSOffTwo, NULL,
                                           DDBLTFAST_WAIT |
                                           DDBLTFAST_SRCCOLORKEY );
					break;

                case VK_F3:
                    if ( ~DestKey ) 
					{
                        OutputDebugString( 
                            "No destination color keying...\n" );
                        break;
                    }
                    OutputDebugString( "Destination color keying...\n" );

                    // Destination keying is supported, so let's set
                    // the primary's key.         
                    ddckey.dwColorSpaceLowValue = dwGreen;
                    ddckey.dwColorSpaceHighValue = dwGreen;

                    lpDDSPrimary->SetColorKey( DDCKEY_DESTBLT, 
                                               &ddckey );

                    // Use a color fill blit to fill the primary surface with
                    // the color key.
                    ZeroMemory( &ddbltfx, sizeof( ddbltfx ) );
                    ddbltfx.dwSize = sizeof( ddbltfx );
                    
                    ddbltfx.dwFillColor = dwGreen; 

                    lpDDSPrimary->Blt( NULL, NULL, NULL,
                                       DDBLT_COLORFILL |
                                       DDBLT_WAIT, &ddbltfx );
                    
                    // Now blit everything in reverse order.
                    lpDDSPrimary->BltFast( 350, 250, lpDDSOffTwo, NULL,
                                           DDBLTFAST_WAIT |
                                           DDBLTFAST_DESTCOLORKEY );

                    lpDDSPrimary->BltFast( 300, 200, lpDDSOffOne, NULL,
                                           DDBLTFAST_WAIT | 
                                           DDBLTFAST_DESTCOLORKEY );

                    lpDDSPrimary->Blt( NULL, lpDDSOffThree, NULL,
                                       DDBLT_WAIT | DDBLT_KEYDEST,
                                       NULL );
                    break;

                case VK_F4:
                    OutputDebugString( "Color fill...\n" );

                    ZeroMemory( &ddbltfx, sizeof( ddbltfx ) );
                    ddbltfx.dwSize = sizeof( ddbltfx );
                    
                    // Fill the surface with blue.
                    ddbltfx.dwFillColor = dwBlue; 

                    lpDDSPrimary->Blt( NULL, NULL, NULL,
                                       DDBLT_COLORFILL |
                                       DDBLT_WAIT, &ddbltfx );
                    break;

                case VK_F5:
                    OutputDebugString( "Mirroring about both axis...\n" );

                    ZeroMemory( &ddbltfx, sizeof( ddbltfx ) );
                    ddbltfx.dwSize = sizeof( ddbltfx );

                    ddbltfx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT |
                                     DDBLTFX_MIRRORUPDOWN;

                    // Mirror the lake on both axis.
                    lpDDSPrimary->Blt( NULL, lpDDSOffThree, NULL,
                                       DDBLT_DDFX |DDBLT_WAIT, 
                                       &ddbltfx );
                    break;

                case VK_F6:
                    OutputDebugString( "Shrinking...\n" );

                    rcRect.left = 0; 
                    rcRect.top = 0; 
                    rcRect.right = 200; 
                    rcRect.bottom = 100; 

                    // Blit the lake with shrinking.
                    lpDDSPrimary->Blt( &rcRect, lpDDSOffThree, NULL,
                                       DDBLT_WAIT, NULL );
                    break;
			}
			break;

		case WM_DESTROY:
			ReleaseObjects();
			PostQuitMessage( 0 );
			break;
		}

    return DefWindowProc( hWnd, message, wParam, lParam );
}

static BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    DDCAPS              ddcaps;
    DDPIXELFORMAT       ddpf;

     // Set up and register window class.
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    // Create a fullscreen window.
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        NAME,
        TITLE,
        WS_POPUP,
        0, 0,
        GetSystemMetrics( SM_CXSCREEN ),
        GetSystemMetrics( SM_CYSCREEN ),
        NULL,
        NULL,
        hInstance,
        NULL );

    if ( !hwnd )
    {
        return FALSE;
    }

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

    // Create the DirectDraw object -- we just need an IDirectDraw
    // interface so we won't bother to query an IDirectDraw2.
    if ( FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
	}

	// Get exclusive mode.
    if ( FAILED( lpDD->SetCooperativeLevel( hwnd,
                        DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) ) )
	{
		return Fail( hwnd, "Couldn't set cooperative level.\n" );
	}

    // Set the display mode. An RGB mode is required for this sample.
    // Try 32 first; if not 32, then 24; if not 24, then 16.
	if ( FAILED( lpDD->SetDisplayMode( 640, 480, 32 ) ) )
	{
        if ( FAILED( lpDD->SetDisplayMode( 640, 480, 24 ) ) )
		{
			if ( FAILED( lpDD->SetDisplayMode( 640, 480, 16 ) ) )
			{
		        return Fail( hwnd, "Couldn't set display mode.\n" );
			}
        }
	}

    // Check for destination color keying cap bit.
    ddcaps.dwSize = sizeof( ddcaps );
    if ( FAILED( lpDD->GetCaps( &ddcaps, NULL ) ) )
	{
        return Fail( hwnd, "Couldn't get caps.\n" );
    }
    if ( ddcaps.dwCKeyCaps | DDCKEYCAPS_DESTBLT ) 
	{
        DestKey = TRUE;
    }

	// Create the primary surface.
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    
	if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create primary surface.\n" );
	}

    // Get the pixel format of the primary and
    // use it to create values for pure green and pure blue.
    ddpf.dwSize = sizeof( ddpf );

	if ( FAILED( lpDDSPrimary->GetPixelFormat( &ddpf ) ) )
	{
		return Fail( hwnd, "Couldn't get the pixel format.\n" );
	}
    dwGreen = ddpf.dwGBitMask;
    dwBlue = ddpf.dwBBitMask;

    // Create the first off-screen surface.
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | 
		           DDSD_WIDTH | DDSD_CKSRCBLT; 
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
    ddsd.dwHeight = 100; 
    ddsd.dwWidth = 100;
    // Set the source color key to green.
    ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = dwGreen;
    ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = dwGreen;

    if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSOffOne, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create off-screen one.\n" );
	}

    // Create the second offscreen surface using the surface 
    // description already set up for the first surface.
    if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSOffTwo, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create offscreen two.\n" );
	}

    // Create the third offscreen surface.
    ddsd.dwHeight = 256; 
    ddsd.dwWidth = 256;
    if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSOffThree, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create offscreen three.\n" );
	}

    // Load our images.
    if ( !myLoadImage( lpDDSOffOne, "redcirq.bmp" ) ) 
	{
        return Fail( hwnd, "Couldn't load offscreen one.\n" );
    }

    if ( !myLoadImage( lpDDSOffTwo, "bluesq.bmp" ) ) 
	{
        return Fail( hwnd, "Couldn't load offscreen two.\n" );
    }

    if ( !myLoadImage( lpDDSOffThree, "lake.bmp" ) ) 
	{
        return Fail( hwnd, "Couldn't load offscreen three.\n" );
    }

	return TRUE;
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if ( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while ( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    return (int)msg.wParam;
}
