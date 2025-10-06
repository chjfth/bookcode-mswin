#define NAME "Inawin"
#define TITLE "Inawin"

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include "ddutil.h"

LPDIRECTDRAW            lpDD;               // DirectDraw object.
LPDIRECTDRAWSURFACE     lpDDSPrimary;       // DirectDraw primary.
LPDIRECTDRAWSURFACE     lpDDSDonut;         // DirectDraw surface.
LPDIRECTDRAWPALETTE     lpDDPalette;        // DirectDraw palette.
LPDIRECTDRAWCLIPPER     lpDDClipper;        // DirectDraw clipper.
BOOL                    bActive;            // Application status.

RECT                    rcWindow;           // Current client area.
HWND                    hWndMain;

DWORD                   dwFrame;            // Current sprite frame.
DWORD                   dwLastTickCount;    // Last frame time.

#define DELAY 15                            // Frame delay.

// Chj: Write cmdline parameter "1" to set g_NOCLIPPER=true .
bool g_NOCLIPPER = false;

static void ReleaseObjects( void )
{
	if ( lpDD != NULL )
	{
		// Our clipper destroys itself automatically
		// when we release the primary surface.
		if ( lpDDSPrimary != NULL ) 
		{
			lpDDSPrimary->Release();
			lpDDSPrimary = NULL;
		}
		// Releasing the primary first insures that
		// the palette will destroy itself.
		if ( lpDDPalette != NULL ) 
		{
			lpDDPalette->Release();
			lpDDPalette = NULL;
		}

		lpDD->Release();
		lpDD = NULL;
	}
}

BOOL Fail( HWND hwnd, char *szMsg )
{
	ReleaseObjects();
	OutputDebugString( szMsg );
	DestroyWindow( hwnd );
	return FALSE;
}

LRESULT WINAPI WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_MOVE:
		// Our window position has changed, so
		// get the client (drawing) rectangle.
		GetClientRect( hWnd, &rcWindow );
		// Convert the coordinates from client relative
		// to screen relative.
		ClientToScreen( hWnd, ( LPPOINT )&rcWindow );
		ClientToScreen( hWnd, ( LPPOINT )&rcWindow + 1 );
		return 0;
		break;

	case WM_SIZE:
		// Our window size is fixed, so this could
		// only be a minimize or maximize.
		if ( wParam == SIZE_MINIMIZED ) 
		{
			// We've been minimized, no need to
			// redraw the screen.
			InvalidateRect( hWnd, NULL, TRUE );
			bActive = FALSE;
		}
		else
		{
			bActive = TRUE;
		}
		return 0;
		break;

	case WM_PALETTECHANGED:
		// First check to see if we caused this message.
		if ( ( HWND )wParam != hWnd ) 
		{
			// We didn't cause it, so we have lost the palette.
			OutputDebugString( "Palette lost.\n" );
			// Realize our palette.
			lpDDSPrimary->SetPalette( lpDDPalette );
			// Convert the sprite image to the new palette.
			DDReLoadBitmap( lpDDSDonut, "donut.bmp" );
		}
		break;

	case WM_QUERYNEWPALETTE:
		// We have control of the palette.
		OutputDebugString( "We have the palette.\n" );
		lpDDSPrimary->SetPalette( lpDDPalette );
		// Convert the sprite image to the new palette.
		DDReLoadBitmap( lpDDSDonut, "donut.bmp" );
		break;

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
			PostMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
		}
		break;

	case WM_DESTROY:
		ReleaseObjects();
		PostQuitMessage( 0 );
		break;

	case WM_DISPLAYCHANGE:
		OutputDebugString( "A setting has changed.\n" );
		break;

	case WM_ACTIVATEAPP:
		// If we wanted to pause the application when it
		// became inactive, we could do so here.
		break;

	case WM_PAINT:
		// We are redrawing every frame so we don't need
		// to process this message. If we were using a
		// pause screen, we could paint it here.
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

BOOL InitDDraw( HWND hwnd )
{
	DDSURFACEDESC       ddsd;       // Surface description structure.
	DDPIXELFORMAT       ddpf;       // Surface format structure.

	// Create the DirectDraw object.
	if ( FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
	}

	// Set normal cooperative level.
	if ( FAILED( lpDD->SetCooperativeLevel( hwnd, DDSCL_NORMAL ) ) )
	{
		return Fail( hwnd, "Couldn't set cooperative level.\n" );
	}

	// Create the primary surface.
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create primary surface.\n" );
	}

	// Create a clipper and attach it to the primary surface.
	if ( FAILED( lpDD->CreateClipper( 0, &lpDDClipper, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create the clipper.\n" );
	}

	// Associate our clipper with our hwnd so it will be updated
	// by Windows.
	if ( FAILED( lpDDClipper->SetHWnd( 0, hwnd ) ) )
	{
		return Fail( hwnd, "Couldn't set the hwnd.\n" );
	}

	// Associate our clipper with the primary surface, so Blt 
	// will use it. If this step is skipped by defining NOCLIPPER, you
	// can observe the effects of rendering without clipping. Other
	// windows partially obscuring Inawin's will be overwritten.

	if(g_NOCLIPPER==false)
	{
		if ( FAILED( lpDDSPrimary->SetClipper( lpDDClipper ) ) )
		{
			return Fail( hwnd, "Couldn't set the clipper.\n" );
		}
	}

	lpDDClipper->Release();

	// Check the surface format.
	ddpf.dwSize = sizeof( ddpf );
	if ( FAILED( lpDDSPrimary->GetPixelFormat( &ddpf ) ) )
	{
		return Fail( hwnd, "Couldn't get pixel format.\n" );
	}

	if ( ddpf.dwFlags & DDPF_PALETTEINDEXED8 ) 
	{
		// We are palettized, so create a palette and attach it to
		// the primary surface.
		lpDDPalette = DDLoadPalette( lpDD, "donut.bmp" );

		if ( FAILED( lpDDSPrimary->SetPalette( lpDDPalette ) ) )
		{
			return Fail( hwnd, "Couldnt' get and/or set the palette.\n" );
		}
	}

	// Create a surface and load our "sprite" into it.
	lpDDSDonut = DDLoadBitmap( lpDD, "donut.bmp", 0, 0, DDSCAPS_OFFSCREENPLAIN);

	if ( lpDDSDonut == NULL )
	{
		return Fail( hwnd, "Could load the donut.\n" );
	}

	return TRUE;
}

BOOL RestoreAll( void )
{
	HRESULT ddrval;

	// Try restoring the primary first
	ddrval = lpDDSPrimary->Restore();

	if ( ddrval == DDERR_WRONGMODE ) 
	{
		// The restore failed because the display mode was
		// changed. Destroy everything then re-create it.
		OutputDebugString( "Display mode changed!\n" );
		ReleaseObjects();
		return InitDDraw( hWndMain );
	}
	else if ( FAILED( ddrval ) )
	{
		// We failed for some other reason.
		OutputDebugString( "Random restore failure.\n" );
		return FALSE;
	}
	else
	{
		// We're OK. Restore everything else.
		if ( FAILED( lpDDSDonut->Restore() ) ) 
			return FALSE;
		
		if ( FAILED( DDReLoadBitmap( lpDDSDonut, "donut.bmp" ) ) ) 
			return FALSE;
	}
	return TRUE;
}

BOOL doInit( HINSTANCE hInstance, int nCmdShow )
{
	HWND        hwnd;
	WNDCLASS    wc;
	RECT        rc1;

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

	// Create a window.
	hwnd = CreateWindowEx(
		0,
		NAME,
		TITLE,
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		NULL,
		NULL,
		hInstance,
		NULL );

	if ( !hwnd )
	{
		return FALSE;
	}

	hWndMain = hwnd;    // Save the window handle.

	// Set the desired size for the client area of the window.
	// The sprite is only 64x64, so the HEL will stretch it.
	SetRect( &rc1, 0, 0, 128, 128 );

	// Adjust that to a size that includes the border, etc.
	AdjustWindowRectEx( &rc1,
		GetWindowStyle( hwnd ),     // Style of our main window.
		GetMenu( hwnd ) != NULL,    // Does the window have a menu?
		GetWindowExStyle( hwnd ) ); // Extended style of the main window.

	// Adjust the window to the new size.
	MoveWindow( hwnd, 
		0, 
		0, 
		rc1.right - rc1.left, 
		rc1.bottom - rc1.top, 
		FALSE );

	// Create all our DirectDraw objects.
	if ( !InitDDraw( hwnd ) ) 
		return FALSE;

	ShowWindow( hwnd, nCmdShow );

	return TRUE;
}

BOOL UpdateFrame( HWND hwnd )
{
	RECT                rcFrame;
	DWORD               dwTickCount;
	HRESULT hret = 0;

	// Check to see if it's time to update the frame.
	dwTickCount = GetTickCount();
	if ( ( dwTickCount - dwLastTickCount ) <= DELAY )
	{
		return TRUE;
	}

	dwLastTickCount = dwTickCount;

	// Update the sprite image with the current frame.
	rcFrame.top = ( ( dwFrame / 5 ) * 64 );
	rcFrame.left = ( ( dwFrame % 5 ) * 64 );
	rcFrame.bottom = rcFrame.top + 64;
	rcFrame.right = rcFrame.left + 64;

	// If the user has changed the display resolution our
	// surfaces could be lost.
	if ( FAILED( lpDDSPrimary->IsLost() ) )
	{
		if ( !RestoreAll() ) 
		{
			return Fail( hwnd, "Couldn't restore surfaces.\n" );
		}
	}

	// Update the window with the new sprite frame. Notice that the
	// destination rectangle is our client rectangle, not the
	// entire primary surface.
	hret = lpDDSPrimary->Blt(&rcWindow, lpDDSDonut, &rcFrame, DDBLT_WAIT, NULL);
	if ( FAILED(hret) )
	{
		return Fail( hwnd, "Couldn't Blt sprite.\n" );
	}

	// Advance the frame for next time.
	dwFrame++;
	if ( dwFrame > 29 ) 
		dwFrame = 0;

	return TRUE;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//[2025-10-04] Chj: Add parameter "1" to set g_NOCLIPPER=1.

	int nArgc = __argc;
#ifdef UNICODE
	PCTSTR* ppArgv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &nArgc);
#else
	PCTSTR* ppArgv = (PCTSTR*) __argv;
#endif
	if(nArgc>1 && _ttoi(ppArgv[1])==1)
		g_NOCLIPPER = true;

	MSG         msg;

	if ( !doInit( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	while ( 1 )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if ( !GetMessage( &msg, NULL, 0, 0 ) )
			{
				return(int)msg.wParam;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if ( bActive )
		{   
			UpdateFrame( hWndMain );
		}
		else
		{
			WaitMessage();
		}
	}
}
