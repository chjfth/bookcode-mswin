#define NAME "Surface"
#define TITLE "DirectDraw Surfaces"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface

BOOL myLoadImage( LPDIRECTDRAWSURFACE lpDDS, LPSTR szImage )
{
	HBITMAP         hbm      = NULL;
	HDC             hdcImage = NULL;
	HDC             hdcSurf  = NULL;
	BOOL            bReturn  = FALSE;
	DDSURFACEDESC   ddsd = {sizeof(ddsd)};

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
		ddsd.dwHeight, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if ( hbm == NULL ) 
	{
		OutputDebugString("Couldn't find the resource.\n" );
		goto Exit;
	}

	// Create a DC and select the image into it.
	hdcImage = CreateCompatibleDC( NULL );
	SelectObject( hdcImage, hbm );

	// Get a DC for the surface.
	if ( FAILED( lpDDS->GetDC( &hdcSurf ) ) )
	{
		OutputDebugString( "Couldn't get a DC.\n");
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
	if ( hbm )
		DeleteObject( hbm );

	return bReturn;
}

static void ReleaseObjects( void )
{
	if ( lpDD != NULL )
	{
		if ( lpDDSPrimary != NULL )
		{
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

LRESULT WINAPI WindowProc( HWND hWnd, UINT message, 
	WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_SETCURSOR:
		SetCursor(NULL);	// Turn off the mouse cursor
		return TRUE;

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
			PostMessage ( hWnd, WM_CLOSE, 0, 0 );
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

	// Set up and register window class
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

	// Create a fullscreen window
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
	// interface so we won't bother to query an IDirectDraw2
	if ( FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
	}

	// Get exclusive mode
	if FAILED( lpDD->SetCooperativeLevel( hwnd,
		DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ) )
	{
		return Fail( hwnd, "Couldn't set cooperative level.\n" );
	}

	// Set the display mode. An RGB mode is required for this sample
	if ( FAILED( lpDD->SetDisplayMode( 640, 480, 32 ) ) )
	{
		return Fail( hwnd, "Couldn't set display mode.\n" );
	}

	// Create the primary surface
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) )
	{
		return Fail( hwnd, "Couldn't create primary surface.\n" );
	}

	if ( !myLoadImage( lpDDSPrimary, "lake.bmp" ) ) 
	{
		return Fail( hwnd, "Couldn't load the image.\n" );
	}

	return TRUE;
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	MSG msg = {};

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
