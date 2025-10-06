#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include "brouhaha.h"

HRESULT DDStartup( LPDIRECTDRAW* lplpDD, GUID FAR* lpGUID,
                   HWND hwnd, BOOL bFullScreen )
{
    HRESULT ddrval = DirectDrawCreate( lpGUID, lplpDD, NULL );
    if FAILED( ddrval ) 
    {
        return ddrval;
    }

    if ( bFullScreen )
    {
        // If fullscreen, get fullscreen exclusive mode
        ddrval = (*lplpDD)->SetCooperativeLevel( hwnd,
                            DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if FAILED( ddrval )
        {
            OutputDebugString( 
				"DDStartup: Couldn't set exclusive mode.\n" );
            return ddrval;
        }

        OutputDebugString( 
			"DDStartup: Setting exclusive mode...\n" );

        if SUCCEEDED( ddrval )
        {
            // Set 640 x 480, 8 bits
            ddrval = (*lplpDD)->SetDisplayMode( 640, 480, 8 );
            return ddrval;
        }
        else
        {
            return ddrval;
        }
    }
    else
    {
        // otherwise, set normal mode for use in a window
        ddrval = (*lplpDD)->SetCooperativeLevel( hwnd, DDSCL_NORMAL );

        OutputDebugString( "DDStartup: Setting windowed mode...\n" );
    }
    return ddrval;
}


HRESULT DDFullConfigure( LPDIRECTDRAW lpDD, 
                        LPDIRECTDRAWSURFACE* lplpDDSPrimary,
                        LPDIRECTDRAWSURFACE* lplpDDSBack )
{
    // Attempt flipping surface with 2 back buffers
    if FAILED ( DDCreateFlipper( lpDD, lplpDDSPrimary, lplpDDSBack, 2 ) )
    {
        // Couldn't get two, try one.
        if FAILED( DDCreateFlipper( lpDD, lplpDDSPrimary, lplpDDSBack, 1 ) )
        {
            // Couldn't even get one. Maybe flipping isn't supported
            // or we have a very small display memory
            if FAILED( DDCreateFakeFlipper( lpDD, lplpDDSPrimary,
                                                    lplpDDSBack) )
            {
                OutputDebugString( 
					"DDFullConfigure: Couldn't create fake flipper.\n" );
                return FALSE;
            }
            else
            {
                OutputDebugString( 
					"DDFullConfigure: Using fake flipper.\n" );
            }
        }
        // Load art where ever it will fit
        return( LoadGameArt( 0, NULL ) );
    }
    else
    {
        if FAILED ( LoadGameArt( DDSCAPS_OFFSCREENPLAIN | 
									DDSCAPS_VIDEOMEMORY, NULL ) )
        {
            // The art wouldn't fit with the double buffers.
            // Release them and back off to a single buffer.
            (*lplpDDSPrimary)->Release();
            if FAILED( DDCreateFlipper ( lpDD, lplpDDSPrimary, lplpDDSBack, 1 ) )
            {
                OutputDebugString( 
					"DDFullConfigure: Single back buffer failed.\n" );
                return FALSE;
            }
            // Load art where ever it will fit
			OutputDebugString( 
				"DDFullConfigure: Using two back buffers, art in display.\n" );
            return( LoadGameArt( DDSCAPS_OFFSCREENPLAIN, NULL ) );
        }
        else
        {
            OutputDebugString( 
				"DDFullConfigure: Using two back buffers, art in display.\n" );
        }
    }
    return TRUE;
}
            
HRESULT DDCreateFlipper( LPDIRECTDRAW lpDD,
                            LPDIRECTDRAWSURFACE* lplpDDSPrimary,
                            LPDIRECTDRAWSURFACE* lplpDDSBack,
                            DWORD dwBufferCount )
{
    HRESULT			ddrval = 0;

    // Create the primary surface with  back buffers
	DDSURFACEDESC	ddsd = {sizeof(ddsd)};
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                              DDSCAPS_FLIP |
                              DDSCAPS_COMPLEX |
                              DDSCAPS_VIDEOMEMORY;
    ddsd.dwBackBufferCount = dwBufferCount;
	ddsd.dwWidth = 640;
	ddsd.dwHeight = 480;

    ddrval = lpDD->CreateSurface( &ddsd, lplpDDSPrimary, NULL );
    if FAILED( ddrval ) 
		return ddrval;
        
    // Get a pointer to the back buffer
	DDSCAPS ddscaps = {DDSCAPS_BACKBUFFER};
    ddrval = (*lplpDDSPrimary)->GetAttachedSurface( &ddscaps, lplpDDSBack );

    return ddrval;
}

HRESULT DDCreateOverFlipper( LPDIRECTDRAW lpDD,
								LPDIRECTDRAWSURFACE* lplpDDSOverlay,
								LPDIRECTDRAWSURFACE* lplpDDSBack,
								DWORD dwBufferCount )
{
    HRESULT			ddrval = 0;

	// We're only going to try one format in Brouhaha. The Overlay
	// sample tries two, and the SDK's Mosquito tries YUV formats
	// as well.

	// Set up the surface format, 16 bit RGB 565
	DDSURFACEDESC ddsd = {sizeof(ddsd)};
	ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwFourCC = 0;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x7C00;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x03e0;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x001F;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;

    // Create the overlay surface with  back buffers
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT | 
					DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY |
                              DDSCAPS_FLIP |
                              DDSCAPS_COMPLEX |
                              DDSCAPS_VIDEOMEMORY;
    ddsd.dwBackBufferCount = dwBufferCount;
	ddsd.dwWidth = 640;
	ddsd.dwHeight = 480;

    ddrval = lpDD->CreateSurface( &ddsd, lplpDDSOverlay, NULL );
    if FAILED( ddrval ) 
		return ddrval;
        
    // Get a pointer to the back buffer
	DDSCAPS ddscaps = {DDSCAPS_BACKBUFFER};
    ddrval = (*lplpDDSOverlay)->GetAttachedSurface( &ddscaps, lplpDDSBack );

    return ddrval;
}

HRESULT DDCreateOverlay( LPDIRECTDRAW lpDD,
                            LPDIRECTDRAWSURFACE* lplpDDSPrimary,
							LPDIRECTDRAWSURFACE* lplpDDSOverlay,
                            LPDIRECTDRAWSURFACE* lplpDDSBack )
{
    HRESULT			ddrval = 0;

	// We will need a primary surface to display the overlay on

	// Create the primary surface
	DDSURFACEDESC ddsd = {sizeof( ddsd )};
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    ddrval = lpDD->CreateSurface( &ddsd, lplpDDSPrimary, NULL );
    if FAILED( ddrval ) 
    {
        OutputDebugString( "DDCreateOverlay: Couldn't create primary.\n" );
        return ddrval;
    }

	// Create the flipping overlay surface

	ddrval = DDCreateOverFlipper( lpDD, lplpDDSOverlay, 
									lplpDDSBack, 1 );
	if FAILED( ddrval )
	{
		OutputDebugString( "DDCreateOverlay: Couldn't create flipper.\n" );
		return ddrval;
	}
	return ddrval;
}

HRESULT DDCreateFakeFlipper( LPDIRECTDRAW lpDD,
                             LPDIRECTDRAWSURFACE* lplpDDSPrimary,
                             LPDIRECTDRAWSURFACE* lplpDDSBack )
{
    HRESULT			ddrval = 0;

    // Create the primary surface
	DDSURFACEDESC ddsd = {sizeof( ddsd )};
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    ddrval = lpDD->CreateSurface( &ddsd, lplpDDSPrimary, NULL );
    if FAILED( ddrval ) 
    {
        OutputDebugString( 
			"DDCreateFakeFlipper: Couldn't create primary.\n" );
        return ddrval;
    }

    // Create an offscreen surface to serve as the backbuffer
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = 640;
    ddsd.dwHeight = 480;
    ddrval = lpDD->CreateSurface( &ddsd, lplpDDSBack, NULL );
    if FAILED( ddrval ) 
    {
        OutputDebugString( 
			"DDCreateFakeFlipper: Couldn't create backbuffer.\n" );
        return ddrval;
    }

    OutputDebugString( "DDCreateFakeFlipper: Using fake flipper.\n" );
    return ddrval;
}

HRESULT DDWinConfigure( LPDIRECTDRAW lpDD,
						LPDIRECTDRAWSURFACE* lplpDDSPrimary,
						LPDIRECTDRAWSURFACE* lplpDDSBack,
						LPDIRECTDRAWCLIPPER* lplpDDClipper,
						LPDIRECTDRAWSURFACE* lplpDDOverlay,
						HWND hWnd )
{
    HRESULT	ddrval;

	ddrval = DDCreateFakeFlipper( lpDD, lplpDDSPrimary, lplpDDSBack );
	if FAILED( ddrval ) 
		return ddrval;

	// Create a clipper and attach it to the primary surface
	ddrval = lpDD->CreateClipper( 0, lplpDDClipper, NULL );
	if FAILED( ddrval ) 
		return ddrval;

	ddrval = (*lplpDDClipper)->SetHWnd( 0, hWnd );
	if FAILED( ddrval ) 
		return ddrval;

	(*lplpDDSPrimary)->SetClipper( *lplpDDClipper );
	
	// So clipper will go away "automatically" when the primary is released.
	(*lplpDDClipper)->Release();

	// Load art where ever it will fit
	return( LoadGameArt( DDSCAPS_OFFSCREENPLAIN, NULL ) );

}

HRESULT DDFillSurface( LPDIRECTDRAWSURFACE lpDDSurface, DWORD color )
{
    DDBLTFX     ddbltfx;
    HRESULT     ddrval;

    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = color;

    ddrval = lpDDSurface->Blt(
                    NULL,                 
                    NULL,          
                    NULL, 
                    DDBLT_COLORFILL | DDBLT_WAIT,
                    &ddbltfx );

    return ddrval;
}

HRESULT DDTextOut( LPDIRECTDRAWSURFACE lpDDSSurface, 
                                char* lpString, 
                                COLORREF BackColor,
                                COLORREF TextColor,
                                int posx,
                                int posy)
{  
    HDC hdc = NULL; 
    HRESULT ddrval = lpDDSSurface->GetDC( &hdc );
    if SUCCEEDED( ddrval )
    {
        SetBkColor( hdc, BackColor );
        SetTextColor( hdc, TextColor);
        TextOut( hdc, posx, posy, lpString, lstrlen(lpString) );
        lpDDSSurface->ReleaseDC( hdc );
    }
    return ddrval;
}

DWORD DDCheckOverlay( GUID *lpGUID )
{
	// A non-zero result indicates overlays are available,
	// and the required stretch factor is returned.
	DWORD dwResult = 0;

	LPDIRECTDRAW	lpDD = NULL;
	if FAILED( DirectDrawCreate( lpGUID, &lpDD, NULL ) )
		return dwResult;

    // Check the hardware caps for "uncomplicated" overlay support

	DDCAPS ddCaps = {sizeof(DDCAPS)};
    lpDD->GetCaps( &ddCaps, NULL );

    if (!( ddCaps.dwCaps & DDCAPS_OVERLAY )) 
		goto CleanUp; 

    // Some type of support is provided, check further
    // We won't tolerate any alignment requirements

    if ( ( ddCaps.dwCaps & DDCAPS_ALIGNBOUNDARYDEST ) ||
         ( ddCaps.dwCaps & DDCAPS_ALIGNBOUNDARYSRC ) ||
         ( ddCaps.dwCaps & DDCAPS_ALIGNSIZEDEST ) ||
         ( ddCaps.dwCaps & DDCAPS_ALIGNSIZESRC ) ) 
    {
		goto CleanUp;
    }

    // Are any overlays available for use?
    if ( ddCaps.dwMaxVisibleOverlays == 
				ddCaps.dwCurrVisibleOverlays )
	{
		goto CleanUp;
	}

    // Finally check for stretching requirements

	if ( ddCaps.dwMinOverlayStretch )
	{
		dwResult = ddCaps.dwMinOverlayStretch;
	}
	else
	{
		dwResult = 1000;
	}

CleanUp:

	lpDD->Release();
    return dwResult;
}
    