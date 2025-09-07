#define NAME "Overlay"
#define TITLE "DirectDraw Overlays"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>

#include "resource.h"

LPDIRECTDRAW            lpDD;               // DirectDraw object.
LPDIRECTDRAWSURFACE     lpDDSPrimary;       // DirectDraw primary surface.
LPDIRECTDRAWSURFACE     lpDDSOverlay;       // DirectDraw overlay surface.
BOOL                    bVisible = FALSE;   // Overlay on or off.
RECT                    rSrc, rDest;        // Overlay rectangles.
DDCAPS                  ddcaps;             // Device caps.

DWORD AlignDown( DWORD dwNumber, DWORD dwAlignment )
{
	DWORD   dwResult;

	if ( dwNumber == 1 ) return dwNumber;

	dwResult = dwNumber - ( dwNumber % dwAlignment );

	return dwResult;
}

DWORD AlignUp( DWORD dwNumber, DWORD dwAlignment )
{
	DWORD   dwResult;

	if ( dwNumber == 1 ) return dwNumber;

	dwResult = dwNumber + ( dwAlignment - 1 );

	dwResult -= dwNumber % dwAlignment;

	return dwResult;
}

BOOL myLoadImage( LPDIRECTDRAWSURFACE lpDDS, LPSTR szImage )
{
	HBITMAP         hbm;
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
	if ( hbm )
		DeleteObject( hbm );

	return bReturn;
}

void ReleaseObjects( void )
{
	if ( lpDD != NULL )
	{
		if ( lpDDSPrimary != NULL )
		{
			if ( lpDDSOverlay != NULL )
			{
				lpDDSOverlay->Release();
				lpDDSOverlay = NULL;
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
	EndDialog( hwnd, FALSE );
	return FALSE;
}

INT_PTR CALLBACK DialogProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{

	DDSURFACEDESC   ddsd;
	DDCOLORKEY      ddckey;     // For SetColorKey.
	DWORD           dwFlags;    // For updating the overlay.

	switch ( message ) 
	{
	case WM_INITDIALOG:

		// Create the DirectDraw object -- we just need an IDirectDraw
		// interface so we won't bother to query an IDirectDraw2.
		if ( FAILED( DirectDrawCreate( NULL, &lpDD, NULL ) ) )
		{
			return Fail( hwnd, "Couldn't create DirectDraw object.\n" );
		}

		// Set normal cooperative levl.
		if ( FAILED( lpDD->SetCooperativeLevel( hwnd, DDSCL_NORMAL ) ) )
		{
			return Fail( hwnd, "Couldn't set cooperative level.\n" );
		}

		// Store the caps in our global and check for overlay cap bit.
		ddcaps.dwSize = sizeof( ddcaps );
		if ( FAILED( lpDD->GetCaps( &ddcaps, NULL ) ) )
		{
			return Fail( hwnd, "Couldn't get caps.\n" );
		}
		if ( !( ddcaps.dwCaps & DDCAPS_OVERLAY ) ) 
		{
			return Fail( hwnd, "No overlay capability.\n" );
		}

		// Check to make sure that an overlay is available for use.
		if ( ddcaps.dwCurrVisibleOverlays == 
			ddcaps.dwMaxVisibleOverlays ) 
		{
			return Fail( hwnd, "No overlays available.\n" );
		}

		// Create the primary surface.
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL ) ) )
		{
			return Fail( hwnd, "Couldn't create primary surface.\n" );
		}

		// Now we'll try to create an overlay surface. We have to guess
		// at the format, as DirectDraw doesn't provide us with a list of
		// possibilities. YUV formats are most common, but we'll try RGB
		// so we don't have to convert our image.

		ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;

		ddsd.dwFlags= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;

		ddsd.dwHeight = 64; 
		ddsd.dwWidth = 64;

		// Set up the first surface format, 16 bit RGB 555.
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
		ddsd.ddpfPixelFormat.dwFourCC = 0;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask = 0x7C00;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x03E0;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x001F;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

		// Try the first format.
		if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSOverlay, NULL ) ) )
		{
			// The first try failed, try again with RGB 565.
			OutputDebugString( "Overlay format RGB555 failed, will try RGB565.\n" );
			ddsd.ddpfPixelFormat.dwRBitMask = 0xF800;
			ddsd.ddpfPixelFormat.dwGBitMask = 0x07E0;
			ddsd.ddpfPixelFormat.dwBBitMask = 0x001F;

			if ( FAILED( lpDD->CreateSurface( &ddsd, &lpDDSOverlay, NULL ) ) )
			{
				return Fail( hwnd, 
					"Couldn't create overlay surface.\n" );
			}
		}

		// If source color key is available for overlays, set a
		// color key of pure green and enable the check box. Use
		// the green bit mask that was successfully created.

		if ( ddcaps.dwCKeyCaps & DDCKEYCAPS_SRCOVERLAY ) 
		{
			ddckey.dwColorSpaceLowValue = ddsd.ddpfPixelFormat.dwGBitMask;

			ddckey.dwColorSpaceHighValue = ddsd.ddpfPixelFormat.dwGBitMask;

			lpDDSOverlay->SetColorKey( DDCKEY_SRCOVERLAY, &ddckey );

			EnableWindow( GetDlgItem( hwnd, IDC_COLORKEY ), TRUE );
		}

		// Load a bitmap into our overlay.
		// Our bitmap is 64 pixels wide, which will satisfy
		// the common alignment requirements of 4 and 8 pixels.

		myLoadImage( lpDDSOverlay, "xrules.bmp" );

		// Prepare the overlay for display by setting the source
		// and destination rectangles. This is the tricky part.

		// We'll start with the upper left corners of both
		// source and destination. Using zeros is the easy
		// way out -- zero aligns with everything.
		rSrc.left = 0; 
		rSrc.top = 0;
		rDest.left = 0; 
		rDest.top = 0; 

		// To show the entire sprite rectangle, we need a bottom
		// right coordinate of 64. Alignment only applies to the right
		// x coordinate if DDCAPS_ALIGNSIZESRC is set in DDCAPS.
		rSrc.bottom = 64; 
		rSrc.right = 64;

		// A value of 64 will probably work, since it aligns with
		// both 4 and 8. If it doesn't, we have to go
		// smaller.
		if ( ddcaps.dwCaps & DDCAPS_ALIGNSIZESRC ) 
		{
			rSrc.right = AlignDown( rSrc.right, ddcaps.dwAlignSizeSrc );
		}

		// The destination rectangle must comply both with
		// alignment and scaling limitations. Again, it's the
		// right X coord that we're concerned with. If you
		// want the aspect ratio to remain correct, you should
		// adjust the bottom or Y coordinate as well.

		// Stretching is specified "times 1000" so that it can be
		// expressed as an integer value. A value of 1000 indicates
		// no stretching. We'd like 4x if we can get it. A value of 0
		// for dwMaxOverlayStretch indicates no maximum limit.

		if ( ( ddcaps.dwMinOverlayStretch <= 4000 ) &&
			( ( ddcaps.dwMaxOverlayStretch >= 4000 ) ||
			ddcaps.dwMaxOverlayStretch == 0 ) )
		{
			// Add 999 to compensate for integer truncation.
			rDest.right = ( ( rSrc.right * 4000 + 999 ) / 1000 );
		}
		else
			// Take the minimum allowable if we can't get 4000.
		{
			// Add 999 to compensate for integer truncation.
			rDest.right = 
				( ( rSrc.right * ddcaps.dwMinOverlayStretch + 999 ) / 1000 );
			
			if ( rDest.right == 0 ) 
				rDest.right = rSrc.right;
		}

		// Align the result, if necessary. Align upward to avoid
		// messing up our minimum stretch adjustment.
		if ( ddcaps.dwCaps & DDCAPS_ALIGNSIZEDEST ) 
		{
			rDest.right = AlignUp( rDest.right, ddcaps.dwAlignSizeDest );
		}

		// Our image is square, so adjust the bottom as well.
		rDest.bottom  = rDest.right;

		break;

	case WM_COMMAND:

		switch ( LOWORD( wParam ) )
		{
		case IDC_SHOW:
			if ( bVisible ) 
			{
				// Hide the overlay.
				if ( FAILED( lpDDSOverlay->UpdateOverlay( NULL, 
					lpDDSPrimary, NULL, DDOVER_HIDE, NULL ) ) )
				{
					return Fail( hwnd, 
						"Couldn't hide the overlay.\n" );
				}
				bVisible = FALSE;
				SetDlgItemText( hwnd, IDC_SHOW, "Show" );
			}
			else
			{
				// If the Use ColorKey check box is checked,
				// include the appropriate flag.
				if ( SendDlgItemMessage( hwnd, IDC_COLORKEY, 
					BM_GETCHECK, 0, 0 ) == BST_CHECKED )
				{
					dwFlags = DDOVER_SHOW | DDOVER_KEYSRC;
				}
				else
				{
					dwFlags = DDOVER_SHOW;
				}

				// Show the overlay.
				if ( FAILED( lpDDSOverlay->UpdateOverlay( &rSrc, 
					lpDDSPrimary, &rDest, dwFlags, NULL ) ) )
				{
					return Fail( hwnd, 
						"Couldn't show the overlay.\n" );
				}

				bVisible = TRUE;
				SetDlgItemText( hwnd, IDC_SHOW, "Hide" );
			}
			break;

		case IDCANCEL:
			EndDialog( hwnd, FALSE );
			return TRUE;
		}
		break;

	case WM_CLOSE:
		EndDialog( hwnd, TRUE );
		return TRUE;

		break;
	}
	return FALSE;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow )
{
	DialogBox( hInstance, MAKEINTRESOURCE( IDD_MAIN ), 
		NULL, ( DLGPROC )DialogProc );

	return TRUE;
}
