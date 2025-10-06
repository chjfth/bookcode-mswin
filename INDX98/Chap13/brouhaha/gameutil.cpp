#include "brouhaha.h"

// These precomputed lookup tables are used to move the ships and shots
// in the directions they're pointed.

static double      Dirx[40] =                           
{
    0.000000,
    0.156434,
    0.309017,
    0.453991,
    0.587785,
    0.707107,
    0.809017,
    0.891007,
    0.951057,
    0.987688,
    1.000000,
    0.987688,
    0.951057,
    0.891007,
    0.809017,
    0.707107,
    0.587785,
    0.453990,
    0.309017,
    0.156434,
    0.000000,
    -0.156435,
    -0.309017,
    -0.453991,
    -0.587785,
    -0.707107,
    -0.809017,
    -0.891007,
    -0.951057,
    -0.987688,
    -1.000000,
    -0.987688,
    -0.951056,
    -0.891006,
    -0.809017,
    -0.707107,
    -0.587785,
    -0.453990,
    -0.309017,
    -0.156434
};

static double      Diry[40] =
{
    -1.000000,
    -0.987688,
    -0.951057,
    -0.891007,
    -0.809017,
    -0.707107,
    -0.587785,
    -0.453990,
    -0.309017,
    -0.156434,
    0.000000,
    0.156434,
    0.309017,
    0.453991,
    0.587785,
    0.707107,
    0.809017,
    0.891007,
    0.951057,
    0.987688,
    1.000000,
    0.987688,
    0.951057,
    0.891006,
    0.809017,
    0.707107,
    0.587785,
    0.453990,
    0.309017,
    0.156434,
    0.000000,
    -0.156435,
    -0.309017,
    -0.453991,
    -0.587785,
    -0.707107,
    -0.809017,
    -0.891007,
    -0.951057,
    -0.987688
};

// Globals

DWORD					g_dwFrameTime = 0;
DWORD					g_dwFrames = 0;
DWORD					g_dwFrameCount = 0;

void DDRelease( void )
{
    if( lpDD != NULL )
    {
        // The clipper and palette, if they exist, will destroy
        // themselves when the primary is released.
        if( lpDDSBack != NULL )
        {
            // This will fail if attempted on a complex surface
            lpDDSBack->Release();
            lpDDSBack = NULL;
        }
        if( lpDDSPrimary != NULL )
        {
            // This will release any associated backbuffers as
            // well, if it's a complex surface.
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        if( lpDDSShips != NULL )
        {   
            lpDDSShips->Release();
            lpDDSShips = NULL;
        }
        if( lpDDSShots != NULL )
        {   
            lpDDSShots->Release();
            lpDDSShots = NULL;
        }
        if( lpDDSGhost != NULL )
        {   
            lpDDSGhost->Release();
            lpDDSGhost = NULL;
        }

        lpDD->Release();
        lpDD = NULL;
    }
}

void CleanUp( void )
{
    DDRelease();
    if( g_hwnd )
    {
        DestroyWindow( g_hwnd );
    }
}

BOOL GameInit( void )
{
	InitLinkedList();

	g_Players[0].lpNode = CreateShip( 100, 100, 0, 0, 0, 0 );

    return TRUE;
}

BOOL CreateSprite(  LPSPRITESET lpSprite,
                    LPDIRECTDRAW lpDD,
                    LPDIRECTDRAWSURFACE* lplpDDSSurface,
                    int stride,
                    int height,
                    int width,
                    LPCSTR szBitmap,
                    LPDDCOLORKEY lpddck,
                    DWORD dwFlags,
					LPDDPIXELFORMAT lpddpfFormat )
{
    HRESULT ddrval;

    // create a surface for the sprite and load the bitmap into it
    (*lplpDDSSurface) = DDLoadBitmap( lpDD, szBitmap, 0, 0, 
											dwFlags, lpddpfFormat );

    if( (*lplpDDSSurface) == NULL )
    {
        OutputDebugString( "Couldn't create art surface.\n" );
        return FALSE;
    }

    // use the color key to make a transparent surface
    if ( lpddck != NULL )
    {
        ddrval = (*lplpDDSSurface)->SetColorKey( DDCKEY_SRCBLT, lpddck );

        if( FAILED( ddrval ) )
        {
            OutputDebugString( "Couldn't set the color key.\n" );
            (*lplpDDSSurface)->Release();
            (*lplpDDSSurface) = NULL;
            return FALSE;
        }
    }

    // now that the surface is ready, fill in the sprite structure
    lpSprite->stride = stride;
    lpSprite->height = height;
    lpSprite->width = width;
    lpSprite->surface = (*lplpDDSSurface);

    return TRUE;
}

HRESULT RestoreSprite( LPSPRITESET lpSprite, LPCSTR szBitmap )
{
    HRESULT ddrval;

    ddrval = ( lpSprite->surface )->Restore();
    if FAILED( ddrval ) return ddrval;

    if FAILED( DDReLoadBitmap( lpSprite->surface, szBitmap ) ) {
        return FALSE;
    }
    return TRUE;
}

HRESULT RestoreSurfaces( void )
{
	HRESULT ddrval;

	// First, try restoring our sprites
	ddrval = RestoreSprite( &g_shipsprite, g_szShipBitmap );
	if FAILED( ddrval ) return ddrval;

	ddrval = RestoreSprite( &g_shotsprite, g_szShotBitmap );
	if FAILED( ddrval ) return ddrval;
	
	ddrval = RestoreSprite( &g_ghostsprite, g_szGhostBitmap );
	if FAILED( ddrval ) return ddrval;

	// Restore the other working surfaces
	if ( lpDDSPrimary ) ddrval = lpDDSPrimary->Restore();

	// If we're using a complex flipping surface with
	// an implicit back buffer, this call will fail. Oh well.
    ddrval = lpDDSBack->Restore();
	if ( FAILED( ddrval ) &&  
		( ddrval != DDERR_IMPLICITLYCREATED ) ) return ddrval;

	return DD_OK;
}

void AttemptRestore( void )
{
	if FAILED( RestoreSurfaces() )
	{
		// If the restore failed, it's time to get
		// serious. Try recreating our DirectDraw
		// objects.

		g_bReInitialize = TRUE;
    
		// Destroy the window and DirectDraw objects
		CleanUp();
    
		// Create a new DirectDraw configuration
		if FAILED( DDInit() )
		{
			OutputDebugString( "Reinitialization failed.\n" );
		}
		ShowWindow( g_hwnd, SW_SHOW );
		g_bReInitialize = FALSE;
	}
}

BOOL CreatePalette( LPDIRECTDRAW lpDD,
                    LPDIRECTDRAWPALETTE* lplpDDPalette,
                    LPDIRECTDRAWSURFACE lpDDSSurface,
                    LPCSTR szBitmap )
{
    (*lplpDDPalette) = DDLoadPalette( lpDD, g_szShipBitmap );

    if( (*lplpDDPalette) == NULL )
    {
        OutputDebugString("Couldn't create palette.\n");
        return FALSE;
    }

    if FAILED( lpDDSSurface->SetPalette( *lplpDDPalette ) )
    {
        OutputDebugString( "Couldn't attach palette.\n" );
        (*lplpDDPalette)->Release();
        (*lplpDDPalette) = NULL;
        return FALSE;
    }

    // Decrement the palette's ref count so it will
    // go away when the surface does.
    (*lplpDDPalette)->Release();

    return TRUE;
}

BOOL LoadGameArt( DWORD dwFlags, LPDDPIXELFORMAT lpddpfFormat ) 
{
    DDCOLORKEY  ddck;
    DDPIXELFORMAT ddpf;

    ddpf.dwSize = sizeof( DDPIXELFORMAT );

    lpDDSPrimary->GetPixelFormat( &ddpf );

    if ( ddpf.dwFlags & DDPF_PALETTEINDEXED8 )
    {
        OutputDebugString( 
			"LoadGameArt: Attaching palette...\n" );
        
        if FAILED( CreatePalette( lpDD, &lpDDPalette, lpDDSPrimary,
                            g_szShipBitmap ) )
        {
            OutputDebugString( 
				"LoadGameArt: Couldn't configure palette.\n" );
            return FALSE;
        }
    }
    else
    {
        OutputDebugString( 
			"LoadGameArt: Non-palettized primary...\n" );
    }

    // set a transparent color key using black
    ddck.dwColorSpaceLowValue = 0x00;
    ddck.dwColorSpaceHighValue = 0x00;

    if FAILED( CreateSprite( &g_shipsprite, lpDD, &lpDDSShips, 10,
                    32, 32, g_szShipBitmap, &ddck, dwFlags, lpddpfFormat ) ) 
    {   
        OutputDebugString( 
			"LoadGameArt: Couldn't load ship sprite.\n" );
        return FALSE;
    }

    if FAILED( CreateSprite( &g_shotsprite, lpDD, &lpDDSShots, 4,
                    3, 3, g_szShotBitmap, &ddck, dwFlags, lpddpfFormat ) ) 
    {   
        OutputDebugString( 
			"LoadGameArt: Couldn't load shot sprite.\n" );
        return FALSE;
    }

	if FAILED( CreateSprite( &g_ghostsprite, lpDD, &lpDDSGhost, 10,
                    32, 32, g_szGhostBitmap, &ddck, dwFlags, lpddpfFormat ) ) 
    {   
        OutputDebugString( 
			"LoadGameArt: Couldn't load ghost sprite.\n" );
        return FALSE;
    }

    OutputDebugString( "LoadGameArt: Art loaded.\n" );
    return TRUE;
}

HRESULT UpdateFrame( BOOL bFull )
{
    char str[255];
    DWORD time, time2;
    HRESULT ddrval = DD_FALSE;

    // Update everyone's position
	UpdateStates();

	time = timeGetTime();

	CheckForHits( g_Players[0].lpNode );

	if ( !bFull ) return TRUE;

	// If it's ok to update the screen, do that too

    if FAILED( DDFillSurface( lpDDSBack, 0 ) )
    {
        OutputDebugString( "UpdateFrame: Couldn't fill back buffer.\n" );
        return ddrval;
    }

    if FAILED( DrawSprites( lpDDSBack,  TRUE ) )
    {
        OutputDebugString( "UpdateFrame: Couldn't draw sprites.\n" );
        return ddrval;
    }

	// Calculate and output the frame rate

    g_dwFrameCount++;
    time2 = time - g_dwFrameTime;
    if( time2 > 1000)
    {
        g_dwFrames = ( g_dwFrameCount * 1000 ) / time2;
        g_dwFrameTime = timeGetTime();
        g_dwFrameCount = 0;
    }
    _sntprintf_s(str, _TRUNCATE, "%d", g_dwFrames);
    DDTextOut( lpDDSBack,str, RGB(0,0,0), RGB(255,255,0), 320, 20 );

    if FAILED( FlipSurfaces( g_dwRenderSetup ) )
    {
        OutputDebugString( "UpdateFrame: Couldn't flip.\n" );
        return ddrval;
    }
    return TRUE;
}

LPNODE CreateShip( double x, double y, double dx, 
						double dy, int offset, int frame )
{
    LPNODE ship;

    ship = (LPNODE) malloc( sizeof(NODE) );
    
    if ( ship == NULL )
        return ship;

    ship->frame = frame;
    ship->offset = offset;
    ship->timeupdate = timeGetTime();
    ship->dwtype = SPRITE_SHIP;
    ship->posx = x;
    ship->posy = y;
    ship->velx = dx;
    ship->vely = dy;
	ship->timedisabled = 0;
	ship->state = UpdateShip;

    ship->spriteset = &g_shipsprite;

    AddNode ( ship );

    return ship;
}

LPNODE CreateShot( double x, double y, double dx, double dy, int offset )
{
    LPNODE shot;

    shot = (LPNODE) malloc( sizeof(NODE) );
    if ( shot == NULL )
        return shot;

    shot->frame = 0;
    shot->offset = offset;
    shot->dwtype = SPRITE_SHOT;
    shot->posx = x;
    shot->posy = y;
    shot->velx = dx;
    shot->vely = dy;
    shot->timeborn = timeGetTime();
    shot->timeupdate = shot->timeborn;
    shot->state = UpdateShot;

    shot->spriteset = &g_shotsprite;

    AddNode ( shot );

    return shot;
}

void UpdateShot( LPNODE shot )
{
    DWORD dwTime;
    double FrameRatio;

    dwTime = timeGetTime();
    FrameRatio = float( dwTime - shot->timeupdate ) / float( FRAME_RATE );

    // shots have a lifetime of SHOTLIFE ms
    if ( ( dwTime - shot->timeborn ) > SHOTLIFE )
    {
        RemoveNode( shot );
        return;
    }

    // calculate a new position based on the time
    // elapsed since the last update

    shot->posx += ( shot->velx * FrameRatio );
    shot->posy += ( shot->vely * FrameRatio );

// Uncomment this section of code if you'd like shots
// to dissappear when they hit the edge of the screen.
// Move the comments to the next section, of course.
/*
    // shots dissapear if they go off the screen/window
    if (  (shot->posx > SHOT_X ) ||
            ( shot->posx < 0 ) ||
            ( shot->posy > SHOT_Y ) ||
            ( shot->posy < 0 ) )
    {
        RemoveNode( shot );
        return;
    }
*/
	// shots reflect off edges of playing field

	if ( shot->posx > SHOT_X )
    {
        shot->posx = SHOT_X;
        shot->velx = -shot->velx;
    }

    if ( shot->posx < 0 )
    {
        shot->posx = 0;
        shot->velx = -shot->velx;
    }

    if ( shot->posy > SHOT_Y )
    {
        shot->posy = SHOT_Y;
        shot->vely = -shot->vely;
    }

    if ( shot->posy < 0 )
    {
        shot->posy = 0;
        shot->vely = -shot->vely;
    }
}

void UpdateShip( LPNODE ship )
{
    DWORD	dwTime, dwDelta;
	double	x, y, dx, dy;
    double	FrameRatio;

    dwTime = timeGetTime();
	dwDelta = dwTime - ship->timeupdate;

    FrameRatio = float ( dwDelta ) / 
                            float ( FRAME_RATE );

    ship->timeupdate = dwTime;

	if ( ( dwTime - ship->timeinput ) > INPUT_RATE )
	{
		ship->timeinput = dwTime;

		if( g_byInput & KEY_LEFT )
		{
			ship->frame -= 1;
			if( ship->frame < 0)
				ship->frame = 39;
		}
		if( g_byInput & KEY_RIGHT )
		{
			ship->frame += 1;
			if( ship->frame > 39 )
				ship->frame = 0;
		}

		if( g_byInput & KEY_THRUST )
		{
			ship->velx += Dirx[ship->frame] / 100;
			ship->vely += Diry[ship->frame] / 100;
		}
	}

    ship->posx += ship->velx * FrameRatio;
    ship->posy += ship->vely * FrameRatio;

    if ( ship->posx > SHIP_X )
    {
        ship->posx = SHIP_X;
        ship->velx = -ship->velx;
    }

    if ( ship->posx < 0 )
    {
        ship->posx = 0;
        ship->velx = -ship->velx;
    }

    if ( ship->posy > SHIP_Y )
    {
        ship->posy = SHIP_Y;
        ship->vely = -ship->vely;
    }

    if ( ship->posy < 0 )
    {
        ship->posy = 0;
        ship->vely = -ship->vely;
    }

    if( g_byInput & KEY_FIRE )
    {
		// You can't fire if you've been disabled
		if ( !( ship->timedisabled ) )
		{
			// Ships can only fire every SHOTFREQ ms.
			if( timeGetTime() - ship->timeborn > SHOTFREQ )
			{
				// Make the shot sort of look like it's coming
				// from the nose of the ship, but not so close we
				// shoot our own foot.
				x = ship->posx + ( Dirx[ship->frame] * 23.0 ) + 16.0;
				y = ship->posy + ( Diry[ship->frame] * 23.0 ) + 16.0;
				// Bias the shot's velocity depending on the ship's
				dx = ( Dirx[ship->frame] * 7.0 ) + ship->velx;
				dy = ( Diry[ship->frame] * 7.0 ) + ship->vely;
				// Add the shot to the linked list and send a message
				CreateShot( x, y, dx, dy, 0 );
				// Store the time the last shot was fired. 
				ship->timeborn = timeGetTime();	
			}
		}
    }

	// See if it's time to re-enable the ship
	if ( ( dwTime - ( ship->timedisabled ) ) > DISABLEDTIME )
	{
		ship->offset = 0;
		ship->spriteset = &g_shipsprite;
		ship->timedisabled = 0;
		ship->status = STATUS_OK;
	}

    return;
}

HRESULT FlipSurfaces( DWORD dwMode )
{
    HRESULT ddrval;

	switch ( dwMode )
	{
		case MODE_OVERLAY:
			ddrval = lpDDSOverlay->Flip( NULL, DDFLIP_WAIT );
			break;

		case MODE_FULL:
			// If we are fullscreen, call Flip as usual
			ddrval = lpDDSPrimary->Flip( NULL, DDFLIP_WAIT );
			break;

		case MODE_WINDOWED:
			// If we are in a window, use Blt instead of Flip.  We must use Blt
			// because BltFast does not perform clipping.
			ddrval = lpDDSPrimary->Blt( &g_rcWindow,
					lpDDSBack,
					NULL,
					DDBLT_WAIT,
					NULL );
			break;
	}

    return ddrval;
}

HWND CreateDesktopWindow(   HINSTANCE hInstance,
                            WNDPROC WindowProc,
                            DWORD dwWidth,  // client area width
                            DWORD dwHeight  // client area height
                        )
{
    WNDCLASS    wc;
    RECT        rc;
    HWND        hwnd;

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
    
    // Create a window
    hwnd = CreateWindowEx(
        0,
        NAME,
        TITLE,
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        SW_SHOW,
        0,
        0,
        NULL,
        NULL,
        hInstance,
        NULL );

    if( !hwnd )
    {
        return NULL;
    }

    // Set the desired size for the *client* area of the window.
    SetRect( &rc, 0, 0, dwWidth, dwHeight );

    // Adjust that to a size that includes the border, etc.
    AdjustWindowRectEx( &rc,
        GetWindowStyle( hwnd ),     // style of our main window
        GetMenu( hwnd ) != NULL,    // does the window have a menu?
        GetWindowExStyle( hwnd ));  // extended style of the main window

    // Adjust the window to the new size
    MoveWindow( hwnd, 
                    CW_USEDEFAULT, 
                    CW_USEDEFAULT, 
                    rc.right-rc.left, 
                    rc.bottom-rc.top, 
                    FALSE );

    return hwnd;
}

HWND CreateFullScreenWindow(HINSTANCE hInstance,
                            WNDPROC WindowProc
                        )
{
    WNDCLASS    wc;
    HWND        hwnd;

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

    if( !hwnd )
    {
        return NULL;
    }

    return hwnd;
}






